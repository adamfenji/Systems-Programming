//Adam Fenjiro
//CS3411 - Program 2
//ctar.c

/* Imports */
#include "ctar.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* Function: main */
int main(int argc, char **argv)
{
    //if the arguments passed are valid
    if (validate_args(argc, argv) != 0){ return 1; }

    //if 'a', append files to the archive
    if (argv[1][1] == 'a'){ handle_append(argv[2], &argv[3]); }
    //if 'd', delete files from the archive
    else if (argv[1][1] == 'd') { handle_delete(argv[2], argv[3]); }

    return 0;
}

/* Function: validate_args
 *  Takes argument count argc and argument list argv
 *
 *  Use this function to perform all error checking
 *  prior to modifying the archive file
 */
int validate_args(int argc, char **argv)
{
    //if the argument count is correct, must be at least 3
    if (argc < 3)
    {
        write(STDERR_FILENO, "Error: Insufficient arguments\n", 30);
        return 1;
    }

    //validate ifn operation is either -a or -d
    if (argv[1][1] != 'a' && argv[1][1] != 'd')
    {
        write(STDERR_FILENO, "Error: Invalid operation\n", 26);
        return 1;
    }

    //if appending -a, make sure that the files exist
    if (argv[1][1] == 'a')
    {
        for (int i = 3; i < argc; i++)
        {
            if (access(argv[i], F_OK) == -1)
            {
                write(STDERR_FILENO, "Error: File not found\n", 23);
                return 1;
            }
        }
    }

    return 0;
}

/* Function: handle_append
 *  Takes the archive file name and a list of files
 *  to add to this file
 *
 *  Fill in the code necessary to correctly add
 *  a header(s) and all file(s) contents of a source file
 *  to an archive file
 */
int handle_append(char *archive_name, char **file_list)
{
    //open or create the archive for reading and writing
    int archive_fd = open(archive_name, O_RDWR | O_CREAT, 0644);
    if (archive_fd == -1)
    {
        write(STDERR_FILENO, "Error: Could not open archive file\n", 36);
        return 1;
    }

    hdr block_header; //header to store file metadata
    memset(&block_header, 0, sizeof(block_header)); //initialize the header with zeros

    //if a valid header exists in the archive?
    if (get_next_header(archive_fd, &block_header) == -1)
    {
        write(STDOUT_FILENO, "Initializing new header...\n", 28);
        block_header.magic = MAGIC_NUMBER;
    }

    //add each file in the file_list to the archive
    for (int i = 0; file_list[i] != NULL; i++)
    {
        add_file_to_header(file_list[i], &block_header);
        write_data_to_archive(archive_fd, &block_header);
    }

    close(archive_fd); //close the archive file descriptor
    return 0;
}

/* Function: handle_delete
 *  Takes the archive file name and a file name to
 *  delete from the archive
 *
 *  Fill in the code necessary to search the archive
 *  file and set the correct flag in the delete array to
 *  indicate the file should not be considered during un-taring
 */
int handle_delete(char *archive_name, char *file)
{
    //open the archive for reading and writing
    int archive_fd = open(archive_name, O_RDWR);
    if (archive_fd == -1)
    {
        write(STDERR_FILENO, "Error: Could not open archive file\n", 36);
        return 1;
    }

    hdr block_header; //header to store file metadata
    //loop through the headers and mark the file as deleted
    while (get_next_header(archive_fd, &block_header) == 0)
    {
        for (int i = 0; i < 8; i++)
        { //read the file name and compare it with the given file
            lseek(archive_fd, block_header.file_name[i], SEEK_SET);
            char archive_file_name[256];
            read(archive_fd, archive_file_name, sizeof(archive_file_name));

            if (strcmp(archive_file_name, file) == 0)
            {
                block_header.deleted[i] = 1; //mark the file as deleted
                lseek(archive_fd, -(off_t)sizeof(hdr), SEEK_CUR);
                write(archive_fd, &block_header, sizeof(hdr));
                break;
            }
        }
    }

    close(archive_fd); //close the archive file descriptor
    return 0;
}

/* Function: add_file_to_header
 *  Takes a source file name to add to the header struct
 *
 *  Fill in the code to append the metadata for a file referenced by
 *  file_name to the header struct block_header
 */
int add_file_to_header(char *file_name, hdr *block_header)
{
    //make sure that the header can store more files
    if (block_header->block_count >= 8)
    {
        write(STDOUT_FILENO, "Header is full, cannot add more files\n", 38);
        return 1;
    }

    //initialize header magic number if necessary
    if (block_header->magic != MAGIC_NUMBER){ block_header->magic = MAGIC_NUMBER; }

    //open the archive to store the file name
    int archive_fd = open("myarchive.ctar", O_RDWR);
    if (archive_fd == -1)
    {
        write(STDERR_FILENO, "Error: Could not open archive file to store file name\n", 54);
        return 1;
    }

    //append the file name to the archive + need to amake sure to update the header
    off_t name_offset = lseek(archive_fd, 0, SEEK_END);
    block_header->file_name[block_header->block_count] = (int)name_offset;

    //write the file name into the archive
    ssize_t written = write(archive_fd, file_name, strlen(file_name) + 1);
    if (written != strlen(file_name) + 1)
    {
        write(STDERR_FILENO, "Error: Could not write file name to archive\n", 44);
        close(archive_fd);
        return 1;
    }

    close(archive_fd); //close the file descriptor
    block_header->block_count++; //increment the block count

    return 0;
}

/* Function: write_data_to_archive
 *  Takes a file descriptor and a pointer to a header struct
 *
 *  Fill in the code to write all header struct contents, and
 *  all file contents which the header refers to, into the
 *  archive file refernced by fd
 */
int write_data_to_archive(int fd, hdr *block_header)
{
    write(fd, block_header, sizeof(hdr)); //write the header to the archive
 
    //for each file in the header, write the data to the archive
    for (int i = 0; i < block_header->block_count; i++)
    {
        if (block_header->deleted[i] == 0)
        {

            lseek(fd, block_header->file_name[i], SEEK_SET);
            char archive_file_name[256];
            int j = 0;
            char c;

            //read the file name from the archive
            while (read(fd, &c, 1) > 0 && c != '\0'){ archive_file_name[j++] = c; }
            archive_file_name[j] = '\0';

            //open the file and write the data into the archive
            int file_fd = open(archive_file_name, O_RDONLY);
            if (file_fd == -1)
            {
                write(STDERR_FILENO, "Error: Could not open file for writing to archive\n", 50);
                return 1;
            }

            char buffer[1024];
            ssize_t bytes;
            //write the file data to the archive, then close the file descriptor
            while ((bytes = read(file_fd, buffer, sizeof(buffer))) > 0){ write(fd, buffer, bytes); }
            close(file_fd);
        }
    }

    return 0;
}

/* Function: get_next_header
 *  Takes a file descriptor and header struct
 *
 *  Fill in the code to get the next header from the file referenced
 *  by fd and write the data into block_header
 */
int get_next_header(int fd, hdr *block_header)
{
    ssize_t bytes_read = read(fd, block_header, sizeof(hdr)); //read the header

    //handle valid and invalid headers
    if (bytes_read > 0)
    {
        if (block_header->magic != MAGIC_NUMBER)
        {
            write(STDOUT_FILENO, "Invalid header read from archive, skipping...\n", 47);
            return -1;
        }

        write(STDOUT_FILENO, "Read valid header from archive\n", 31);
        for (int i = 0; i < block_header->block_count; i++)
        {
            char offset_str[20];
            write(STDOUT_FILENO, "File name offset in header: ", 28);
            write(STDOUT_FILENO, offset_str, strlen(offset_str));
            write(STDOUT_FILENO, "\n", 1);
        }
        return 0;
    }
    //if no headers found, initialize a new one
    else if (bytes_read == 0)
    {
        write(STDOUT_FILENO, "No headers found, initializing a new header...\n", 47);
        memset(block_header, 0, sizeof(hdr));
        block_header->magic = MAGIC_NUMBER;
        block_header->block_count = 0;
        return -1;
    }
    else
    {
        write(STDERR_FILENO, "Error reading from archive\n", 28);
        return -1;
    }
}

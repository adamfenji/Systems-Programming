//Adam Fenjiro
//CS3411 - Program 2
//utar.c

/* Imports */
#include "utar.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

/* Function: main */
int main(int argc, char **argv)
{

    //if the command-line arguments are valid
    if (validate_args(argc, argv) != 0){ return 1; }

    //get the archive name and call the utar handler
    char *archive_name = argv[1];
    if (handle_utar(archive_name) != 0){ return 1; }

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
    //make sure the archive file is specified as an argument
    if (argc < 2)
    {
        write(STDERR_FILENO, "Error: Insufficient arguments\n", 30);
        return 1;
    }
    //if the archive exists
    if (access(argv[1], F_OK) == -1)
    {
        write(STDERR_FILENO, "Error: Archive file does not exist\n", 35);
        return 1;
    }
    //if it is accessible for reading
    if (access(argv[1], R_OK) == -1)
    {
        write(STDERR_FILENO, "Error: Cannot read archive file\n", 32);
        return 1;
    }

    return 0;
}

/* Function: handle_utar
 *  Takes an archive_name to extract the contents of
 *
 *  Write the code to extract all files from archive_name
 *  into the current directory
 */
int handle_utar(char *archive_name)
{
    //open the archive for reading
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd == -1)
    {
        write(STDERR_FILENO, "Error: Could not open archive file\n", 36);
        return 1;
    }

    hdr block_header; //header to read metadata
    //loop through the headers and extract each file
    while (get_next_header(archive_fd, &block_header) == 0)
    {
        for (int i = 0; i < block_header.block_count; i++)
        {
            if (block_header.deleted[i] == 0)
            {
                char file_name[256] = {0};
                off_t name_offset = block_header.file_name[i];

                //seek to the file name's position and read it
                lseek(archive_fd, name_offset, SEEK_SET);
                ssize_t name_len = read(archive_fd, file_name, sizeof(file_name) - 1);

                //open or create the file to extract
                int file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (file_fd == -1)
                {
                    write(STDERR_FILENO, "Error: Could not create file: ", 30);
                    write(STDERR_FILENO, file_name, strlen(file_name));
                    write(STDERR_FILENO, "\n", 1);
                    return 1;
                }

                //buffer to read the file data and write it to the new file
                char buffer[1024];
                ssize_t bytes;
                off_t file_size = block_header.file_size[i];
                while (file_size > 0 && (bytes = read(archive_fd, buffer, sizeof(buffer))) > 0)
                {
                    if (bytes > file_size)
                        bytes = file_size;
                    write(file_fd, buffer, bytes);
                    file_size -= bytes;
                }

                close(file_fd); //close the file descriptor
            }
        }
    }

    close(archive_fd); //close the archive file descriptor
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
    //read the next header from the archive
    ssize_t bytes_read = read(fd, block_header, sizeof(hdr));

    //if end of file reached
    if (bytes_read == 0)
    {
        return -1;
    }

    //error or incomplete header read
    else if (bytes_read != sizeof(hdr))
    {
        write(STDERR_FILENO, "Error: Failed to read header\n", 29);
        return -1;
    }

    return 0; //its valid
}

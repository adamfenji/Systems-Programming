//CS3411 Fall 24 - Systems Programming
//Program 4 - Husky Script (hscript)
//Adam Fenjiro - afenjiro@mtu.edu

//Imports
#include <unistd.h> //fork(), pipe(), dup2(), execvp()
#include <stdio.h> //fprintf(), perror()
#include <fcntl.h> //open(), fcntl()
#include <string.h> //strlen(), strerror()
#include <stdlib.h> //malloc(), free(), exit()
#include <limits.h> //PATH_MAX
#include <errno.h> //errors
#include <sys/wait.h> //waitpid()
#include <sys/stat.h> //stat(), mkdir()
#include <sys/select.h> //select()

//Helper function to handle all the input and output stuff using non-blocking I/O
void helper_io(int source, int destination, int logg) {
    
    fd_set read_fds; //File descriptor
    char buffer[4096]; //Buffer to store data read
    struct timeval timeout; //Timeout for select()

    // Set source file descriptor to non-blocking
    fcntl(source, F_SETFL, O_NONBLOCK);
    while (0 == 0) {
        FD_ZERO(&read_fds); // Clear the file descriptor set
        FD_SET(source, &read_fds); // Add the source file descriptor to the set

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; //timeout = 100ms

        //wait for source file descriptor to become ready
        if (select(source + 1, &read_fds, NULL, NULL, &timeout) == -1) break;

        //retry after timeout
        if (select(source + 1, &read_fds, NULL, NULL, &timeout) == 0) continue;

        //if source file descriptor has data available
        if (FD_ISSET(source, &read_fds)) {
            ssize_t bytes_read = read(source, buffer, sizeof(buffer));
            if (bytes_read <= 0) break; //break on error or EOF
            if (destination >= 0) write(destination, buffer, bytes_read); //standard output
            if (logg >= 0) write(logg, buffer, bytes_read); //log in specified directory
        }
    }
}

//Helper function to open log file in the specified directory with the given filename
int helper_open_log_file(const char *directory, const char *filename) {
    char full_path[PATH_MAX]; //Buffer to store the full path to the log file
    snprintf(full_path, sizeof(full_path), "%s/%s", directory, filename);
    int file_descriptor = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644); //Open the log file with write permission
    return file_descriptor;
}

int main(int argc, char *argv[]) {
    
    //Not all arg provided, help user
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <program_name> <arguments> <directory>\n", argv[0]);
        return 1;
    }

    //Get the other arg
    const char *program_name = argv[1];
    const char *directory = argv[argc - 1];

    // Check or create directory
    struct stat directory_info = {0}; //store directory information

    //if the path exits, then create the dir with permissions 0755
    if (stat(directory, &directory_info) == -1) { 
        if (mkdir(directory, 0755) == -1) return -1;
    } 
    else if (!S_ISDIR(directory_info.st_mode)) { //else if the path exist but no dir
        fprintf(stderr, "Error: %s exists but is not a directory\n", directory);
        return -1;
    }

    // Create pipes for stdin, stdout, and stderr
    int input_pipe[2], output_pipe[2], error_pipe[2];
    if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1 || pipe(error_pipe) == -1) return 1;

    //Fork a child process
    pid_t child_process_id = fork();

    if (child_process_id == -1) return 1; //fork failure
    if (child_process_id == 0) { // Child process
        //Close unused write end pipes
        close(input_pipe[1]);
        close(output_pipe[0]);
        close(error_pipe[0]);

        //Redirect pipes to standard input, output, and error
        dup2(input_pipe[0], STDIN_FILENO);
        dup2(output_pipe[1], STDOUT_FILENO);
        dup2(error_pipe[1], STDERR_FILENO);

        //Close remaining pipe ends
        close(input_pipe[0]);
        close(output_pipe[1]);
        close(error_pipe[1]);

        // Prepare arguments for exec
        char **new_arg_values = malloc(sizeof(char *) * (argc - 1));
        for (int i = 1; i < argc - 1; i++) new_arg_values[i - 1] = argv[i]; //Copy arguments to then Null terminate the list
        new_arg_values[argc - 2] = NULL;

        //Execute the program, free memory ,then exit
        execvp(program_name, new_arg_values);
        free(new_arg_values);
        exit(1);
    }

    //Parent process
    //Close unused write end pipes
    close(input_pipe[0]);
    close(output_pipe[1]);
    close(error_pipe[1]);

    //Using helper, open log files
    int input_log = helper_open_log_file(directory, "0");
    int output_log = helper_open_log_file(directory, "1");
    int error_log = helper_open_log_file(directory, "2");

    //errors when opening log files
    if (input_log == -1 || output_log == -1 || error_log == -1) {
        close(input_pipe[1]);
        close(output_pipe[0]);
        close(error_pipe[0]);
        return 1;
    }

    //helper for i o for stdout and stderr
    helper_io(output_pipe[0], STDOUT_FILENO, output_log);
    helper_io(error_pipe[0], STDERR_FILENO, error_log);

    //Close pipes and log files
    close(output_pipe[0]);
    close(error_pipe[0]);
    close(output_log);
    close(error_log);
    close(input_log);

    //Wait for child process to finish, then exit
    return WEXITSTATUS(waitpid(child_process_id, NULL, 0));
}
//CS3411 Fall 24 - Systems Programming
//Program 5 - RPC, rserver.c
//Adam Fenjiro - afenjiro@mtu.edu

//Imports
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//Opcodes for RPC calls
#define open_call 1
#define close_call 2
#define read_call 3
#define write_call 4
#define seek_call 5

void rp_open(int connection){
    char buffer[4];

    //Read the length of the pathname
    if (read(connection, buffer, 2) < 0) perror("ERROR: Failed to read pathname length");
    int length = (buffer[0] << 8) | buffer[1];

    //Read the actual pathname
    char *path = malloc(length);
    if (read(connection, path, length) < 0) perror("ERROR: Failed to read pathname");

    memset(buffer, 0, 4);

    //Read the flags for opening the file
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read flags");
    int flags = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    memset(buffer, 0, 4);

    //Read the mode for file permissions
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read mode");
    int mode = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Open operation and store the file descriptor
    int fd = open(path, flags, mode);
    if (fd < 0) perror("ERROR: Failed to open file");

    int index = 0;
    char message[8];

    //Write the file descriptor to the response message
    message[index++] = (fd >> 24) & 0xff;
    message[index++] = (fd >> 16) & 0xff;
    message[index++] = (fd >> 8) & 0xff;
    message[index++] = (fd) & 0xff;

    //Write errno value to the response message
    message[index++] = (errno >> 24) & 0xff;
    message[index++] = (errno >> 16) & 0xff;
    message[index++] = (errno >> 8) & 0xff;
    message[index++] = (errno) & 0xff;

    // Send the response back to the client side
    write(connection, message, 8);

    free(path);
}

void rp_close(int connection){
    char buffer[4];

    //Read the file descriptor
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read file descriptor");
    int fd = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Close operation
    int close_out = close(fd);
    if (close_out < 0) perror("ERROR: Failed to close file descriptor");

    int index = 0;
    char message[8];

    //Write the result of close to the response message
    message[index++] = (close_out >> 24) & 0xff;
    message[index++] = (close_out >> 16) & 0xff;
    message[index++] = (close_out >> 8) & 0xff;
    message[index++] = (close_out) & 0xff;

    //Write errno value to the response message
    message[index++] = (errno >> 24) & 0xff;
    message[index++] = (errno >> 16) & 0xff;
    message[index++] = (errno >> 8) & 0xff;
    message[index++] = (errno) & 0xff;

    //Send the response back to the client side
    write(connection, message, 8);
}

void rp_read(int connection){
    char buffer[4];

    //Read the file descriptor
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read file descriptor");
    int fd = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Read the count of bytes to read
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read count");
    int count = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    char *buffer2 = malloc(count);
    if (!buffer2) perror("ERROR: Failed to allocate memory for read buffer");

    //Read operation
    int read_out = read(fd, buffer2, count);
    if (read_out < 0){
        perror("ERROR: Failed to read data from file descriptor");
        free(buffer2);
    }

    int index = 0;
    char *message = malloc(read_out + 8);
    if (!message){
        perror("ERROR: Failed to allocate memory for response message");
        free(buffer2);
    }

    //Write the number of bytes read to the response message
    message[index++] = (read_out >> 24) & 0xff;
    message[index++] = (read_out >> 16) & 0xff;
    message[index++] = (read_out >> 8) & 0xff;
    message[index++] = (read_out) & 0xff;

    //Write errno value to the response message
    message[index++] = (errno >> 24) & 0xff;
    message[index++] = (errno >> 16) & 0xff;
    message[index++] = (errno >> 8) & 0xff;
    message[index++] = (errno) & 0xff;

    //Append the read data to the response message
    for (int i = 0; i < read_out; i++) message[index++] = buffer2[i];

    //Send the response back to the client side
    write(connection, message, read_out + 8);

    free(buffer2);
    free(message);
}

void rp_write(int connection){
    char buffer[4];
    
    //Read the file descriptor
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read file descriptor");
    int fd = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Read the count of bytes to write
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read count");
    int count = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Read data
    char *data = malloc(count);
    if (!data) perror("ERROR: Failed to allocate memory for write buffer");

    //Read the data to be written
    if (read(connection, data, count) < 0) perror("ERROR: Failed to read data");

    //Write operation
    int write_out = write(fd, data, count);
    if (write_out < 0){
        perror("ERROR: Failed to write data to file descriptor");
        free(data);
    }

    int index = 0;
    char message[8];

    //Write the number of bytes written to the response message
    message[index++] = (write_out >> 24) & 0xff;
    message[index++] = (write_out >> 16) & 0xff;
    message[index++] = (write_out >> 8) & 0xff;
    message[index++] = (write_out) & 0xff;

    //Write errno value to the response message
    message[index++] = (errno >> 24) & 0xff;
    message[index++] = (errno >> 16) & 0xff;
    message[index++] = (errno >> 8) & 0xff;
    message[index++] = (errno) & 0xff;

    //Send the response back to the client side
    write(connection, message, 8);
}

void rp_seek(int connection){
    char buffer[4];
    
    //Read the file descriptor
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read file descriptor");
    int fd = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Read the offset for seek
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read offset");
    int offset = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //Read the whence value
    if (read(connection, buffer, 4) < 0) perror("ERROR: Failed to read whence");
    int whence = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    //lseek operation
    int seek_out = lseek(fd, offset, whence);
    if (seek_out < 0) perror("ERROR: Failed to perform seek on file descriptor");

    int index = 0;
    char message[8];

    //Write the resulting offset to the response message
    message[index++] = (seek_out >> 24) & 0xff;
    message[index++] = (seek_out >> 16) & 0xff;
    message[index++] = (seek_out >> 8) & 0xff;
    message[index++] = (seek_out) & 0xff;

    //Write errno value to the response message
    message[index++] = (errno >> 24) & 0xff;
    message[index++] = (errno >> 16) & 0xff;
    message[index++] = (errno >> 8) & 0xff;
    message[index++] = (errno) & 0xff;

    //Send the response back to the client side
    write(connection, message, 8);
}

//Helper function to handle the RPC call type
void helper_call(int connection, char rpc_call){
    if (rpc_call == open_call) rp_open(connection);
    else if (rpc_call == close_call) rp_close(connection);
    else if (rpc_call == read_call) rp_read(connection);
    else if (rpc_call == write_call) rp_write(connection);
    else if (rpc_call == seek_call) rp_seek(connection);
}

int main(int argc, char *argv[]){
    struct sockaddr_in server_address, client_address;

    //Create a socket for listening to connections
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) perror("ERROR: Failed to create socket"); 

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(0);

    //Bind the socket to a random port
    if (bind(listener, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) perror("ERROR: Failed to bind socket");

    //Get and display the assigned port number
    socklen_t server_length = sizeof(server_address);
    getsockname(listener, (struct sockaddr *)&server_address, &server_length);
    printf("Port Number: %d\n", ntohs(server_address.sin_port));

    //Start listening for connections
    if (listen(listener, 1) < 0) perror("ERROR: Failed to listen on socket"); 

    //Infinite loop to accept and handle client connections
    while (0 == 0)
    {
        socklen_t client_length = sizeof(client_address);
        int connection = accept(listener, (struct sockaddr *)&client_address, &client_length);
        if (fork() == 0) //Child
        {
            close(listener);
            char rpc_call;
            do{
                if (read(connection, &rpc_call, 1) <= 0)
                {
                    perror("ERROR: Failed to read RPC call");
                    close(connection);
                    exit(1);
                }
                helper_call(connection, rpc_call); //Helper
        } while (rpc_call != EOF);
            exit(1);
        }
        close(connection); //Parent process closes the connection
    }
    return 0;
}
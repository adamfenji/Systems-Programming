//CS3411 Fall 24 - Systems Programming
//Program 5 - RPC, rclient.c
//Adam Fenjiro - afenjiro@mtu.edu

//Imports
#include "rclient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//Opcodes for RPC calls
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5

//Global socket file descriptor
int connection_socket;

int rp_open(const char *pathname, int flags, int mode){
	
	const char *p = pathname; //Pointer to traverse pathname
	while(*p) p++; //Move pointer to the end of the string

	int index = (p - pathname) + 11; //Calculate message size
	char *message = malloc(index); //Allocate memory for the message
	index = 0;

	//Add opcode for 'open', pathname for high and low bytes
	message[index++] = (open_call) & 0xff;
	message[index++] = ((p - pathname) >> 8) & 0xff;
	message[index++] = (p - pathname) & 0xff;

	// Add pathname characters
	for (int i = 0; i < (p - pathname); i++) message[index++] = (pathname[i]) & 0xff;

	//Add flags
	message[index++] = (flags >> 24) & 0xff;
	message[index++] = (flags >> 16) & 0xff;
	message[index++] = (flags >> 8) & 0xff;
	message[index++] = (flags ) & 0xff;

	//Add mode
	message[index++] = (mode >> 24) & 0xff;
	message[index++] = (mode >> 16) & 0xff;
	message[index++] = (mode >> 8) & 0xff;
	message[index++] = (mode ) & 0xff;

	//Send the message
	write(connection_socket, message, index);
	read(connection_socket, message, 8);

	int response = (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | message[3]; //Parse response
	errno = (message[4] << 24) | (message[5] << 16) | (message[6] << 8) | message[7]; //Parse errno

	free(message);

	return response;
}

int rp_close(int fd){

	int index = 0;
	char *message = malloc(8); //Allocate memory for the message

	//Add opcode for 'close' and file descriptor
	message[index++] = (close_call) & 0xff;
	message[index++] = (fd >> 24) & 0xff;
	message[index++] = (fd >> 16) & 0xff;
	message[index++] = (fd >> 8) & 0xff;
	message[index++] = (fd ) & 0xff;

	write(connection_socket, message, index); //Send the message
	read(connection_socket, message, 8); //Read response

	message = malloc(8);
	int response = (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | message[3]; //Parse response
	errno = (message[4] << 24) | (message[5] << 16) | (message[6] << 8) | message[7]; //Parse errno

	free(message);

	return response;
}

int rp_read(int fd, void *buffer, int count){

	int index = 0;
	char *message = malloc(9); //Allocate memory for the message

	//Add opcode for 'read' and file descriptor
	message[index++] = (read_call) & 0xff;
	message[index++] = (fd >> 24) & 0xff;
	message[index++] = (fd >> 16) & 0xff;
	message[index++] = (fd >> 8 ) & 0xff;
	message[index++] = (fd 		) & 0xff;

	//Add byte count
	message[index++] = (count >> 24) & 0xff;
	message[index++] = (count >> 16) & 0xff;
	message[index++] = (count >> 8 ) & 0xff;
	message[index++] = (count	   ) & 0xff;

	write(connection_socket, message, index); //Send message

	message = malloc(8 + count); //Reallocate memory to read response
	read(connection_socket, message, count + 8); //Read response

	int response = (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | message[3]; //Parse response
	errno = (message[4] << 24) | (message[5] << 16) | (message[6] << 8) | message[7]; //Parse errno

	//Copy data into the buffer
	for(int i = 0; i < response; i++) ((char *)buffer)[i] = message[i+8];

	free(message);

	return response;
}

int rp_write(int fd, const void *buffer, int count){
	
	int index = 0;
	char *message = malloc(9 + count); //Allocate memory for the message

	//Add opcode for 'write' and file descriptor
	message[index++] = (write_call) & 0xff;
	message[index++] = (fd >> 24) & 0xff;
	message[index++] = (fd >> 16) & 0xff;
	message[index++] = (fd >> 8 ) & 0xff;
	message[index++] = (fd 		) & 0xff;

	//Add bytes count
	message[index++] = (count >> 24) & 0xff;
	message[index++] = (count >> 16) & 0xff;
	message[index++] = (count >> 8 ) & 0xff;
	message[index++] = (count	   ) & 0xff;

	//Copy data from the buffer
	for (int i = 0; i < count; i++) message[index++] = (((const char *)buffer)[i]) & 0xff;

	write(connection_socket, message, index);  //Send the message
	read(connection_socket, message, 8);  //Read response

	message = malloc(8);
	int response = (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | message[3]; //Parse response
	errno = (message[4] << 24) | (message[5] << 16) | (message[6] << 8) | message[7]; //Parse errno

	free(message);

	return response;
}

int rp_seek(int fd, int offset, int whence){

	int index = 0;
	char *message = malloc(13); //Allocate memory for the message

	//Add opcode for 'seek' and file descriptor
	message[index++] = (seek_call) & 0xff;
	message[index++] = (fd >> 24) & 0xff;
	message[index++] = (fd >> 16) & 0xff;
	message[index++] = (fd >> 8 ) & 0xff;
	message[index++] = (fd 		) & 0xff;

	//Add offset
	message[index++] = (offset >> 24) & 0xff;
	message[index++] = (offset >> 16) & 0xff;
	message[index++] = (offset >> 8 ) & 0xff;
	message[index++] = (offset	   ) & 0xff;

	//Add whence
	message[index++] = (whence >> 24) & 0xff;
	message[index++] = (whence >> 16) & 0xff;
	message[index++] = (whence >> 8 ) & 0xff;
	message[index++] = (whence	   ) & 0xff;

	write(connection_socket, message, index);  //Send message
	read(connection_socket, message, 8);  //Read response

	message = malloc(8);
	int response = (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | message[3]; //Parse response
	errno = (message[4] << 24) | (message[5] << 16) | (message[6] << 8) | message[7]; //Parse errno

	free(message);

	return response;
}

// Entry point for rclient 1 and 2
int entry(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    
	if (argc < 5) {
        printf("./rclient <hostname> <portnumber> <input_file> <output_file>\n");
        return 0;
    }

	//Create connection socket
    connection_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in remote = {0}; //Initialize remote server address
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(argv[2])); //Port Number

    struct hostent *h = gethostbyname(argv[1]); //Resolve host
    bcopy((char *)h->h_addr, (char *)&remote.sin_addr, h->h_length); //Copy host address

	//Connect to server
    connect(connection_socket, (struct sockaddr *)&remote, sizeof(remote));

    char *eargv[] = {argv[0], argv[3], argv[4]}; //Arguments for entry
    int result = entry(argc - 2, eargv); //Call entry()

    char eof = EOF; //Signal end of communication
    write(connection_socket, &eof, 1);
    return result;
}
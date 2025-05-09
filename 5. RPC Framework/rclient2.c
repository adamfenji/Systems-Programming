//CS3411 Fall 24 - Systems Programming
//Program 5 - RPC, rclient2.c
//Adam Fenjiro - afenjiro@mtu.edu

//Imports
#include "rclient.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

//Entry point function for rclient2
int entry(int argc, char* argv[]){

	//Remote and local file paths
	const char* remote_file_path = argv[1];
    const char* local_file_path = argv[2];
	
	int remote_file = rp_open(remote_file_path, O_RDONLY | O_CREAT, 0666); //Open remote file for reading
	int local_file = open(local_file_path, O_RDWR | O_CREAT | O_TRUNC, 0666); //Open local file for writing
	
	//Check if remote file was successfully opened
	if (remote_file < 0) perror("ERROR: Failed to open remote file");

	//Check if local file was successfully opened
    if (local_file < 0) {
        perror("ERROR: Failed to open local file");
        rp_close(remote_file);
    }

	char buffer[64]; //Buffer to store file content
	rp_seek(remote_file, 10, SEEK_SET); //Skip the first 10 bytes
	
	//Read initial chunk from remote file and write to local file
	int read_out = rp_read(remote_file, buffer, 64);
	write(local_file, buffer, read_out);

	//Continue reading and writing until the end of the file
	while(read_out > 0){
		read_out = rp_read(remote_file, buffer, 64);
		write(local_file, buffer, read_out);
	}

	//Close both files
	rp_close(remote_file);
	close(local_file);
	
	return 0; 
}
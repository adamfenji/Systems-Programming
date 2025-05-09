//CS3411 Fall 24 - Systems Programming
//Program 5 - RPC, rclient1.c
//Adam Fenjiro - afenjiro@mtu.edu

//Imports
#include "rclient.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

//Entry point function for rclient1
int entry(int argc, char* argv[]){

	//Remote and local file paths
	const char* remote_file_path = argv[2];
    const char* local_file_path = argv[1];

	int local_file = open(local_file_path, O_RDONLY | O_CREAT, 0666); //Open local file for reading
	int remote_file = rp_open(remote_file_path, O_RDWR | O_CREAT | O_TRUNC, 0666); //Open remote file for writing

	//Check if remote file was successfully opened
	if (remote_file < 0) perror("ERROR: Failed to open remote file");

	//Check if local file was successfully opened
    if (local_file < 0) {
        perror("ERROR: Failed to open local file");
        rp_close(remote_file); //Close if failed
    }
	
	char buffer[64]; //Buffer to store file content
	int read_out = read(local_file, buffer, 64); //Read from local file
	rp_write(remote_file, buffer, read_out);  //Write the initial chunk to the remote file
	
	//Continue reading and writing until the end of the file
	while(read_out > 0){
		read_out = read(local_file, buffer, 64);
		rp_write(remote_file, buffer, read_out);
	}

	//Close both files
	rp_close(remote_file);
	close(local_file);
	
	return 0;
}
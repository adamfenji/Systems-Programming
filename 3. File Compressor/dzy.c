//CS3411 Fall 24 - Systems Programming
//Program 3 - Compressing Files
// Adam Fenjiro - afenjiro@mtu.edu

//dzy.c

#include "bitsy.h"

#define EOF_CODE 258 //used to indicate end of file or read errors

//helper to retrieve a 3-bit shift value
//logic: read 3 bits one by one and accumulate them to form a shift value
unsigned short get_shift() {
    unsigned short shift = 0;
    for (int i = 0; i < 3; i++) {
        shift = (shift << 1) | read_bit(); //shift left and add the new bit
    }
    return shift;
}

//helper to add a char to buffer of the last 8 bytes, used to track recent bytes for frequent byte pattern decoding
//logic: insert new byte in a circular buffer that stores only the last 8 bytes
void add_buffer(char *buffer, unsigned int *count, char entry) {
    buffer[*count % 8] = entry;
    (*count)++;
}

/* main - dzy de-compression implementation
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){

	char buffer[8] = {0}; //buffer to store the last 8 bytes for pattern matching
    unsigned int count = 0; //counter to track position in buffer
    unsigned short read_bit_, read_byte_;

	while(0 == 0){
		read_bit_ = read_bit();
		if (read_bit_ == EOF_CODE) break; //end of file

		if(read_bit_ == 1){ //infrequent character encoding
			read_byte_ = read_byte();
			if (read_byte_ == EOF_CODE) break; //end of file
			write_byte(read_byte_); //byte to output
			add_buffer(buffer, &count, read_byte_);
		}
        else { // frequent character encoding
            read_bit_ = read_bit();
            if (read_bit_ == EOF_CODE) break; //end of file

            unsigned short shift = get_shift(); //get 3-bit shift for position
            if (read_bit_ == 0) { // single frequent byte and no repetition
                char retrieved = buffer[(count - (shift + 1)) % 8];
                write_byte(retrieved);
                add_buffer(buffer, &count, retrieved);
            } else if (read_bit_ == 1) { //multiple repeated bytes
                char repeat = buffer[(count - (shift + 1)) % 8];
                write_byte(repeat);
                add_buffer(buffer, &count, repeat);

                shift = get_shift(); //get the num of repeats
                for (int i = 0; i <= shift; i++) { //write + add each repeated byte
                    write_byte(repeat);
                    add_buffer(buffer, &count, repeat);
                }
            } else { //error here, nor 0 oula 1
                return -1;
            }
        }
    }
    return 0;
}
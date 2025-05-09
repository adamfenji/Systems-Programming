//CS3411 Fall 24 - Systems Programming
//Program 3 - Compressing Files
// Adam Fenjiro - afenjiro@mtu.edu

//czy.c

#include "bitsy.h"

#define EOF_CODE 258 //used to indicate end of file or read errors

//helper to check if a byte is unique in the last 8 bytes in buffer
//logic: compares the current byte with each of the last 8 bytes to find matches
char is_unique(char *buffer, unsigned int count, char read_byte_) {
    for (int i = 0; i <= 7; i++) {
        if (buffer[(count - (i + 1)) % 8] == read_byte_) {
            return i;
        }
    }
    return 9;
}

//helper to add a char to buffer of the last 8 bytes, used to track recent bytes for frequent byte pattern decoding
//logic: insert new byte in a circular buffer that stores only the last 8 bytes
void add_buffer(char *buffer, unsigned int *count, char entry) {
    buffer[*count % 8] = entry;
    (*count)++;
}

//helper to write 3 bit value to the file bit by bit used for small values
//logic: shifts bits and writes each bit individually to output
void write_3bit(unsigned short value) {
    write_bit((value & 0x4) >> 2); //leftmopst
    write_bit((value & 0x2) >> 1); //middle
    write_bit(value & 0x1); //rightmost
}

/* main - czy compression implementation
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	
    char buffer[8] = {0}; //store the last 8 bytes for pattern checking
    int last = 0; //flag to track if last byte was a repeat
    unsigned int count = 0; //counter to track position in buffer
    char final = 0; //store the last repeated byte

	while(0 == 0){ 

		char read_byte_ = 0;
		if(!last){ //if not a repeat sequence then read new byte 
			unsigned short out_read = read_byte();
			if(out_read == EOF_CODE) break; //end of file
			read_byte_ = (char) out_read;
		}
        else{read_byte_ = final;} //repeat sequence then use last repeated byte

        //check if the current byte is unique within the last 8 bytes
		char unique = is_unique(buffer, count, read_byte_);

        //if unique byte then write 1 bit followed by the byte
		if(unique >= 9){
			write_bit(1);
			write_byte(read_byte_);
			add_buffer(buffer, &count, read_byte_); //add byte to buffer
			last = 0;
		}
        else{ //else frequent byte then write 0 bit followed by its relative position
			write_bit(0); 
			unsigned short repeat = read_byte();
			if(repeat == EOF_CODE){ //end of file
				write_bit(0);
                write_3bit(unique);
                break;
			}
            else if(read_byte_ == (char)repeat){ //if repeated byte matches next read byte
				add_buffer(buffer, &count, repeat); //add to buffer twice for repetition tracking
				add_buffer(buffer, &count, repeat); //add to buffer twice for repetition tracking
				// add_buffer(buffer, &count, repeat);
                char repeat_count = 0;
				while (repeat_count < 8) { //count consecutive repeats = up to 8
                    repeat = read_byte();
                    if (repeat == EOF_CODE || (char) repeat != read_byte_) break; //stop if no match
                    add_buffer(buffer, &count, (char) repeat); //add match
                    repeat_count++;
                }

				if (repeat_count == 8) repeat_count = 7; //max repeat count to 7
				
                write_bit(1); //write match position and repated
                write_3bit(unique);
                write_3bit(repeat_count);

				if (repeat == EOF_CODE) break; //end of file

                last = 1; //set repeat flag, then store repeated
                final = repeat;
			}
            else{
				write_bit(0); //write match position and add buffer
                write_3bit(unique);
                add_buffer(buffer, &count, read_byte_);

                last = 1; //set repeat flag, then store repeated
                final = repeat;
			}
		}
	}
    //flush remaining bits in buffer
	flush_write_buffer();
	return 0;
}
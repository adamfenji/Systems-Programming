//CS3411 Fall 24 - Systems Programming
//Program 3 - Compressing Files
// Adam Fenjiro - afenjiro@mtu.edu

//bitsy.c

#include "bitsy.h"
#include <unistd.h>

#define EOF_CODE 258 //used to indicate end of file or read errors

//Reads a single bit from the file
//logic: return the next bit from the buffer / the bit at the next position if partial
char buff = 0; //buffer to store a full byte temporarily
char available = 0; //tracks how many bits remain in the buffer
unsigned short read_bit() {
    if (available == 0) { 
        if (read(0, &buff, 1) == 0) return EOF_CODE; //refill buffer if no bits are available
        available = 8;
    }

    unsigned short result = (buff & 0x80) >> 7; //isolate the next bit = leftmost
    buff <<= 1;
    available--;
    return result;
}

//Reads a full byte from the file
//logic: use read_bit() to read each bit toaccumulate them into a single byte and return it
unsigned short read_byte() { 
    char read_byte = 0;
    for (int i = 0; i < 8; i++) {
        unsigned short result = read_bit();
        if (result > 255) return EOF_CODE; //if end of file
        read_byte = (read_byte << 1) | result; //accumulate bit into read_byte
    }
    return read_byte;
}

//Writes a single bit to the file.
//logic: if buffer accumulates 8 bits then the byte is written to the file and the buffer is reset
char write_buff = 0; //buffer to accumulate bits into a full byte
int needed = 8; //tracks how many bits are needed to fill the buffer
void write_bit(unsigned char bit) {
    write_buff = (write_buff << 1) | bit;
    needed--;

    if (needed == 0) { //if buffer is full = 8 bits, then write full
        write(1, &write_buff, 1);
        needed = 8;
    }
}

//Writes a full byte to the file
//logic: if the buffer is aligned then it writes the byte directly, else writes each bit individually
void write_byte(unsigned char byt) {
    for (int i = 0; i < 8; i++) {
        unsigned char bit = (byt & 0x80) >> 7; //get the leftmost bit, then shift, then write
        byt <<= 1;
        write_bit(bit);
    }
}

//any remaining bits in the buffer will be written to the file as a partial byte, especially the final byte that do not align with 8 bits
void flush_write_buffer(){
	if(needed == 8) return; //just empty 

	int neededCount = needed; //track how many bits remain to fill byte
	while(neededCount > 0){
		write_bit(1);
		neededCount--;
	}
}

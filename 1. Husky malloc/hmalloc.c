//Adam Fenjiro
//CS3411
//Program 1 - hmalloc.c

#include "hmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

void *free_list = NULL;

void traverse() {

    if (!free_list) return; //Base case

	int traverse_the_list = 1;
	int index = 0;
	void *current = free_list;
    
    while (traverse_the_list) {

		/* Printing format:
		* "Index: %d, Address: %08x, Length: %d\n"
		* -Index is the position in the free list for the current entry.
		* 0 for the head and so on
		* -Address is the pointer to the beginning of the area.
		* -Length is the length in bytes of the free area.
		*/
		printf("Index: %d, ", index);
		printf("Address: %08x, ", (unsigned int)((uintptr_t)current + 8));
		printf("Length: %d\n", *((int *)(current)));

        //Check if the offset to the next block is 0, then break the loop
        if (*((int *)(current + 4)) == 0) { 
			traverse_the_list = 0; 
		}
		else{ //We keep moving to the next block
			current = (char *)current + *((int *)(current + 4));
       		index++;
		}
        
    }
}

void *hmalloc(int bytes_to_allocate){
	
	int bytes_needed = bytes_to_allocate + 8; //8 bytes for metadata
	void* pointer;

	//If there are free blocks in the free list
	if(free_list){
		
		//Keeping track of the nodes
		void* current_node = free_list;
		void* previous_node = NULL;

		//Traverse the free list to find a large enough block
		while(((int*)current_node)[0] < bytes_to_allocate){
			
			//If this is the last block, we break the loop
			if (((int*)current_node)[1] == 0) {  break; }
				
			//Move to the next block in the free list
			previous_node = current_node;
    		current_node = (char*)current_node + ((int*)current_node)[1];

		}

		//If current node is still too small
		if (((int*)current_node)[0] < bytes_to_allocate) {
    		pointer = sbrk(bytes_needed);
    		if (pointer == NULL) { return NULL; } //Handle memory allocation failure
			
            //Initialize the metadata for the new block
    		int* metadata = (int*)pointer;
   			metadata[0] = bytes_to_allocate;
    		metadata[1] = 0;
		}
		else{ //If a suitable block is found, I will use the current block
    		pointer = current_node;

    		int next_offset = ((int*)current_node)[1];

    		if (next_offset == 0 && previous_node == NULL) { 
				//if only one block in the free list, set free_list to NULL
				free_list = NULL; 
			} 
			else if (previous_node == NULL) { 
				//if the block being used is the head of the free list, move the head to the next block
				free_list = (char*)current_node + next_offset; 
			} 
			else if (next_offset == 0) { 
				//If the block being used is the last block, update the previous block's offset to 0
				((int*)previous_node)[1] = 0; 
			} 
			else { 
				//if the block is in the middle, link the previous block to the next block
				((int*)previous_node)[1] += next_offset; 
			}
		}

		return pointer + 8; //skip the metadata
	}
	else{
		pointer = sbrk(bytes_needed);
    	if (pointer == NULL) { return NULL; } //Handle memory allocation failure
    
		//Initialize the metadata for the new block
    	int* metadata = (int*)pointer;
   		metadata[0] = bytes_to_allocate;
    	metadata[1] = 0;
	}

   return pointer + 8; //skip the metadata
}

void *hcalloc(int bytes_to_allocate){
	
	void* pointer = hmalloc(bytes_to_allocate); // Allocate memory using hmalloc
    if (pointer == NULL) { return NULL;} // Return NULL if memory allocation fails
	
	//Access the metadata
    int* metadata = (int*)(pointer - 8);
    int size = metadata[0];
	
	//Clear the allocated memory (use memset instead of a loop)
    memset(pointer, 0, size);
	
   	return pointer;
}

void hfree(void *ptr){

	if (ptr == NULL) return; //base case

	ptr = (char*)ptr - 8; //for metadata

	//If the free list is empty
    if (free_list == NULL) {
        free_list = ptr;
        ((int*)ptr)[1] = 0;  //Set the offset to 0 to be at the end of the list
    } 
	else { //Add the block to the front of the free list
        int* metadata = (int*)ptr;
        int offset_to_free_list = (char*)free_list - (char*)ptr;  //Calculate offset to the current free list head
        metadata[1] = offset_to_free_list;
        free_list = ptr;
    }
}

//For extra credit
void* hrealloc(void* ptr, int new_size) {

    if (ptr == NULL) { return hmalloc(new_size); } //base case, use hmalloc

    //If new_size is 0, we need to free the memory and return NULL
    if (new_size == 0) { 
		hfree(ptr);
        return NULL;
    }

    //Move pointer back 8 bytes to access metadata
    int* old_metadata = (int*)((char*)ptr - 8);
    int old_size = old_metadata[0];

    // If the new size is smaller or equal to the old size, we need to return the same block
    if (new_size <= old_size) { return ptr; }

    //new block with new size
    void* new_ptr = hmalloc(new_size);
    if (new_ptr == NULL) { return NULL; }

    // Copy the old data to the new block
    memcpy(new_ptr, ptr, old_size);
    hfree(ptr); // Free the old block

    return new_ptr;
}
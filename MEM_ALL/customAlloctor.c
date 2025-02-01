#include "customAllocator.h"
#include <unistd.h>
#include <stdlib.h>

// use of blockList to create the linked list
// when the program executes there's always a program break

// get the current program break
void* initial_prog_brk = sbrk(0);

// assign the program break to the block list pointer and initialize the size to 0
block_list = (blockList*)sbrk(sizeof(blockList));
if (block_list == SBRK_FAIL) {
    perror("error"); // need to change to the correct print
}
block_list->block_arr = NULL;
block_list->total_size = 0;
block_list->allocated_size = 0;
block_list->blocks_num = 0;
block_list->allocated_blocks = 0;


void* customMalloc(size_t size) {
    // check for incorrect parameters
    if (size <= 0) {
        perror("<malloc error>: passed nonpositive size");
        return NULL;
    }

    size_t available_size = block_list->total_size - block_list->allocated_size;
    size_t available_blocks = block_list->blocks_num - block_list->allocated_blocks;
    Block* free_block = findFirstFreeBlock(size);
    // the memory block have enough memory for the request allocation
    if (available_blocks > 0 && available_size > size && free_block) {
        
        // check for available blocks
        size_t size_diff = free_block->size - size; // set the diffrent to default
        Block* match_block = free_block;
        // go through all of the blocks
        for (int i = 0; i < block_list->blocks_num; i++) {
            if (block_list->block_arr[i]->free) {

                size_t curr_size_diff = block_list->block_arr[i]->size - size;
                if(curr_size_diff < size_diff && curr_size_diff >= size) { // found closer block from the default
                    match_block = block_list->block_arr[i];
                } 
            }
        } // end for

    match_block->free = false;
    block_list->allocated_blocks++;
    block_list->allocated_size += match_block->size;
    return (void*)(match_block + 1);
    
        
    } else {
        // the block list is empty or any existing block didn't match, so we need to allocate new
        // increment the size of the program break by size bytes
        size_t aligned_size = ALIGN_TO_MULT_OF_4(size)
        void* new_prog_brk = sbrk(aligned_size);
        if (new_prog_brk == SBRK_FAIL) {
            perror("error"); // change the to correct printing
        }

        //creates new block to assign
        Block* new_block = (Block*)sbrk(sizeof(Block));
        if (new_block == SBRK_FAIL) {
            perror ("error"); // need to change to the correct print
            return NULL;
        }
        new_block->size = aligned_size;
        new_block->next = NULL;
        new_block->free = false;

        if(block_list->list_size > 0) {
            Block* block_it = block_list->block_arr;
            while(block_it->next != NULL) {
                block_it = block_it->next;
            }

            block_it->next = new_block;
        } else {
            block_list->block_arr = new_block;
        }

        block_list->total_size += aligned_size ;
        block_list->allocated_size += aligned_size ;
        block_list->blocks_num++;
        block_list->allocated_blocks++;

        block_list->block_arr = (Block*)new_prog_brk;
        return (void*)block_list->block_arr;
    }
}

void customFree(void* ptr);
void* customCalloc(size_t nmemb, size_t size);
void* customRealloc(void* ptr, size_t size);

Block* findFirstFreeBlock (size_t req_size) {
    Block* block_it = block_list->block_arr;
    while (block_it != NULL) {
        if (block_it->free && block_it->size >= req_size) {
            return block_it;
        }
        block_it = block_it->next;
    }
    return NULL;
}
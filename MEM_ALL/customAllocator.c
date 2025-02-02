#define _DEFAULT_SOURCE
#include "customAllocator.h"
#include <stdlib.h>

// use of blockList to create the linked list
// when the program executes there's always a program break

// get the current program break
//void* initial_prog_brk = sbrk(0);

// Forwared declaration of helper functions
static Block* findFirstFreeBlock(size_t size);
bool areAdjacentBlocks(Block* block1, Block* block2);
void freeAllMemory();

// Create a blockList to store the blocks - set as static object
static void* initial_prog_brk = NULL;
static blockList* block_list = NULL;

__attribute__((constructor)) 
static void initAlloctor(void){
    // Save the initial program break
    initial_prog_brk = (void*)sbrk(0);
    block_list = (blockList*)sbrk(sizeof(blockList));
    if (block_list == SBRK_FAIL) {
        perror("error"); // need to change to the correct print
    }

    block_list->block_arr = NULL;
    block_list->total_size = 0;
    block_list->allocated_size = 0;
    block_list->blocks_num = 0;
    block_list->allocated_blocks = 0;
}

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
        Block* block_it = block_list->block_arr;
        while (block_it->next != NULL) {
            if (block_it->free) {
                size_t curr_size_diff = block_it->size - size;
                if(curr_size_diff < size_diff && curr_size_diff >= size) { // found closer block from the default
                    match_block = block_it;
                } 
            }
            block_it = block_it->next;
        }

    match_block->free = false;
    block_list->allocated_blocks++;
    block_list->allocated_size += match_block->size;
    return (void*)(match_block + 1);
    
        
    } else {
        // the block list is empty or any existing block didn't match, so we need to allocate new
        // increment the size of the program break by size bytes
        size_t aligned_size = ALIGN_TO_MULT_OF_4(size); 

        //creates new block to assign
        Block* new_block = (Block*)sbrk(sizeof(Block) + aligned_size);
        if (new_block == SBRK_FAIL) {
            perror ("error"); // need to change to the correct print
            return NULL;
        }
        new_block->size = aligned_size;
        new_block->next = NULL;
        new_block->free = false;

        if (block_list->total_size > 0) {
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

        //block_list->block_arr = (Block*)new_prog_brk;
        return (void*)(new_block + 1);
    }
}

void customFree(void* ptr){
    // Check if given pointer is NULL
    if (ptr == NULL){
        perror("<free error>: passed null pointer");
        return;
    }

    // Set the pointer to the block to free to the start of the header of the block
    Block* block_to_free = (Block*)ptr - 1;

    // Mark the block as free if not already free and uptade the allocated blocks and size
    if (!block_to_free->free){
        block_to_free->free = true;
        block_list->allocated_blocks--;
        block_list->allocated_size -= block_to_free->size;
    }

    // Try to merge blocks in memory if possible
    Block* block_it = block_list->block_arr;
    while (block_it != NULL && block_it->next != NULL){
        // If the current and next blocks are free and adjacent, merge them
        if (block_it->free && block_it->next->free && areAdjacentBlocks(block_it, block_it->next)){
            // New size = current size + header + size of next block
            block_it->size += sizeof(Block) + block_it->next->size;
            block_it->next = block_it->next->next;
            block_list->blocks_num--;
        } else {
            block_it = block_it->next;
        }
    }

    // When arriving to the last block, try to shrink the program break if possible
    
    // Get the last block in the blocks list
    Block* last_block = block_list->block_arr;
    Block* prev_block = NULL;
    while (last_block != NULL && last_block->next != NULL){
        prev_block = last_block;
        last_block = last_block->next;
    }

    if (last_block != NULL && last_block->free){
        size_t size_to_shrink = sizeof(Block) + last_block->size;
        
        // Try to shrink the program break, if not possible print error and free all the memory
        void* result = sbrk(-((intptr_t)size_to_shrink));
        if (result == SBRK_FAIL){
            if (errno == ENOMEM){
                perror("<sbrk erorr>: out of memory");

                // Call function to free all memory blocks and exit 
                freeAllMemory();
                exit(1);
            } 
        } else {
            if (prev_block != NULL)
                prev_block->next = NULL; // Set current last block to NULL
            else // Reached the first block
                block_list->block_arr = NULL;
            
            // Update the block list's metadata
            block_list->blocks_num--;
            block_list->total_size -= size_to_shrink;
        }
    }
}
   

// Helper function to check if 2 blocks are adjacent to one another
bool areAdjacentBlocks(Block* block1, Block* block2){
    return (block1 + block1->size == block2);
}

// Helper function to free all memory blocks
void freeAllMemory(){
    // Set program break to the initial program break
    brk(initial_prog_brk);

    // Reset the block list metadata
    block_list->block_arr = NULL;
    block_list->total_size = 0;
    block_list->allocated_size = 0;
    block_list->blocks_num = 0;
    block_list->allocated_blocks = 0;     
}

// Allocates memory for array of nmemb elements with size bytes each and set all to 0
void* customCalloc(size_t nmemb, size_t size){
    // Check if passed nonpositive value
    if (nmemb <= 0 || size <= 0){
        perror("<calloc error>: passed nonpositive value");
        return NULL;
    }

    // Calculate the total size of allocation
    size_t total_size = nmemb * size;
    
    void* ptr = customMalloc(total_size);
    if (ptr == NULL)
        return NULL;
    
    // Set all the allocated memory bytes to 0
    memset(ptr, 0, total_size);

    return ptr;
}   

// size < old size
// (0) / header: sizeof(Block) / data ... / ... / ... size ... / ... / old size 

//changes the size of  the block
void* customRealloc(void* ptr, size_t size) {
    if (!ptr) {
        return customMalloc(size);
    }

    Block* block = ((Block*)ptr) - 1;
    size_t old_size = block->size;

    if (size == old_size) {
        return ptr;
    }

    if (size < old_size) { //free the excess memory
        size_t excess_mem = old_size - size;
        // call customFree for the ptr offset?
        
        // If we have enough excess memory, we create a new block to split the old one
        if (excess_mem >= (sizeof(Block) + 1)){
            // Set the size of the current block to the new size
            block->size = size;

            // Set the new block to the excess memory
            Block* new_free_block = (Block*)((char*)block + sizeof(Block) + size);
            new_free_block->size = excess_mem - sizeof(Block);
            new_free_block->free = true;
            new_free_block->next = block->next;
            block->next = new_free_block;

            // Update the block list's metadata
            block_list->blocks_num++;
            block_list->allocated_size -= new_free_block->size;

            return ptr;
        } else { // In case there is not enough excess memory
            // Allocate new memory block with the new size
            void* new_ptr = customMalloc(size);
            
            // Check allocation success
            if (new_ptr == NULL)
                return NULL;

            // Copy the data from the old block to the new one and delete the old block
            memcpy(new_ptr, ptr, size);
            customFree(ptr);
            return new_ptr;
        }
    } else { // size > old_size
        // Allocate new memory block with the new size
        void* new_ptr = customMalloc(size);

        // Check allocation success
        if (new_ptr == NULL)
            return NULL;
        
        // Copy the data from the old block to the new one and delete the old block
        memcpy(new_ptr, ptr, old_size);
        customFree(ptr);
        return new_ptr;
    }
        
    //     if (!customFree(ptr + sizeof(Block) + )) {
    //          // if free failed
    //         Block* new_block = (Block*)customMalloc(size);
    //         memcpy((new_block+1), (Block*)(ptr+1), size);
    //         customFree(ptr);
    //         block->size = size;
    //         return (void*)(new_block + 1);
    //     }
    // } else { // size > old_size
    //     Block* new_block = (Block*)customMalloc(size);
    //     memcpy((new_block + 1), (Block*)(ptr+1),size);
    //     custromFree(ptr);
    //     return (void*)(new_block + 1);
    // }
   
}

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
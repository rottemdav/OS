#ifndef __CUSTOM_ALLOCATOR__
#define __CUSTOM_ALLOCATOR__

/*=============================================================================
* do no edit lines below!
=============================================================================*/
#include <stddef.h> //for size_t
#include <stdbool.h> //for bool
#include <unistd.h> //for sbrk
#include <errno.h> //for errno
#include <string.h> //for memcpy
#include <stdio.h> //for perror
#include <stdint.h> //for intptr_t

void* customMalloc(size_t size);
void customFree(void* ptr);
void* customCalloc(size_t nmemb, size_t size);
void* customRealloc(void* ptr, size_t size);
/*=============================================================================
* do no edit lines above!
=============================================================================*/

typedef struct Block
{
    size_t size;
    struct Block* next;
    bool free;
} Block;

typedef struct blockList {
    size_t total_size;
    size_t allocated_size;
    Block* block_arr;
    size_t blocks_num;
    size_t allocated_blocks;

} blockList;

//extern blockList* block_list;


 

/*=============================================================================
* if writing bonus - uncomment lines below
=============================================================================*/
// #ifndef __BONUS__
// #define __BONUS__
// #endif
// void* customMTMalloc(size_t size);
// void customMTFree(void* ptr);
// void* customMTCalloc(size_t nmemb, size_t size);
// void* customMTRealloc(void* ptr, size_t size);

// void heapCreate();
// void heapKill();

/*=============================================================================
* defines
=============================================================================*/
#define SBRK_FAIL (void*)(-1)
#define ALIGN_TO_MULT_OF_4(x) (((((x) - 1) >> 2) << 2) + 4)



#endif // CUSTOM_ALLOCATOR

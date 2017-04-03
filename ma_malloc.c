#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "ma_malloc.h"


#define MEM_POOL_SIZE 600 //in bytes
typedef unsigned char byte;
typedef int size;
typedef enum {ALLOCATED,FREE} mem_status;

typedef struct {
  size size;
  mem_status status;
} mem_chunk_header;

static byte mem_pool[MEM_POOL_SIZE];


/*
** Allocates array of bytes (memory pool) and initializes the memory allocator. 
** If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
*/
void ma_init() {
	printf("\nma_init called");
	mem_chunk_header *header = (mem_chunk_header *) (&mem_pool[0]);
	mem_chunk_header *footer = (mem_chunk_header *) (((byte *) (&mem_pool[MEM_POOL_SIZE-1])) -sizeof(mem_chunk_header)+1);
	header->status = FREE;
	header->size = MEM_POOL_SIZE -2*sizeof(mem_chunk_header);
	footer->status = FREE;
	footer->size = header->size;
	// here comes your code
}

/*
** Requesting for the tsize bytes from memory pool. 
** If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
*/
void* ma_malloc(size tsize) {
	printf("\nma_malloc called");
	int i = 0;
	mem_chunk_header *free_header = NULL;
	mem_chunk_header *next_header;
	mem_chunk_header *next_footer;
	size remaining_free = 0;
	mem_chunk_header *footer;
	mem_chunk_header *header;
	
	while(i < MEM_POOL_SIZE) {
		header = (mem_chunk_header *) (&mem_pool[i]);
		if(header->size >= tsize && header->status == FREE) {
			//if a block is found, write its info into free_header
			free_header = header;
			printf("\n	free block found");
			break;
		} else {
			//check next block by updating header
			i += (header->size + 2*sizeof(mem_chunk_header));
		}
	}
	if(!free_header){
		printf("\n	no block found");
		return NULL;
	}
	
	remaining_free = free_header->size - tsize - 2*sizeof(mem_chunk_header);
	if(remaining_free > 0) {
		printf("\n	free space remaining: %d", remaining_free);
		//if extra space in block, set address of next header and footer, its status to free and its size to the remainder 
		next_header = (mem_chunk_header *) ((byte *) (free_header) + tsize + 2*sizeof(mem_chunk_header));
		next_header->status = FREE;
		next_header->size = remaining_free;
		next_footer = (mem_chunk_header *) ((byte *) (next_header) + remaining_free + sizeof(mem_chunk_header));
		next_footer->status = FREE;
		next_footer->size = next_header->size;
		//set location of footer to just before next header
		footer = next_header - sizeof(mem_chunk_header);
	} else {
		footer = (mem_chunk_header *) ((byte *) (free_header) + tsize + sizeof(mem_chunk_header));
	}
	free_header->status = ALLOCATED;
	free_header->size = tsize;
	footer->status = ALLOCATED;
	footer->size = header->size;
	byte  *ptr = (byte *) (free_header) + sizeof(mem_chunk_header);
	return ptr;
}

/*
** Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses. 
** Implement also the coalescing behavior.
*/
void ma_free(void* ptr) {
	printf("\nma_free called");
	//SCOPECHECK
	if((byte *) ptr <= (byte *) &mem_pool[MEM_POOL_SIZE-1] && (byte *) ptr >= (byte *) &mem_pool[0]){
		printf("\n	*(pointer in scope)");
	
		mem_chunk_header *header = (mem_chunk_header *) ((byte *)(ptr) - sizeof(mem_chunk_header));
		mem_chunk_header *footer = (mem_chunk_header *) ((byte *)(ptr) + header->size);
		header->status = FREE;
		footer->status = FREE;
		printf("\n	chunk freed");
		
		//Check if a presceding footer is still in the scope of our heap->previous chunk exists or not
		if((byte *) (header)-2*sizeof(mem_chunk_header)>(byte *) (&mem_pool[0])) {
			printf("\n	*(previous chunk in scope)");
			mem_chunk_header *prev_footer = (mem_chunk_header *) ((byte *) (header) - 2*sizeof(mem_chunk_header));
			mem_chunk_header *prev_header = (mem_chunk_header *) ((byte *) (prev_footer) -(prev_footer->size + sizeof(mem_chunk_header)));
			if(prev_footer->status == FREE) {
				printf("\n	previous chunk is free");
				prev_header->status = FREE;
				prev_header->size = header->size + prev_footer->size + 2*sizeof(mem_chunk_header);
				footer->size = prev_header->size;
				header = prev_header;
				printf("\n	coalesced with previous chunk, new size = %d", header->size);
			}
		}
		if((byte *) (header)+header->size +2*sizeof(mem_chunk_header)<(byte *) (&mem_pool[MEM_POOL_SIZE-1])) {
			printf("\n	*(next chunk in scope)");
			mem_chunk_header *next_header = (mem_chunk_header *) ((byte *) (header) + header->size + 2*sizeof(mem_chunk_header));
			mem_chunk_header *next_footer = (mem_chunk_header *) ((byte *) (next_header) +next_header->size + sizeof(mem_chunk_header));
			if(next_header->status == FREE) {
				printf("\n	next chunk is free");
				header->size = (header->size + next_header->size + 2*sizeof(mem_chunk_header));
				next_footer->status = FREE;
				next_footer->size = header->size;
				footer = next_footer;
				printf("\n	coalesced with next chunk, new size = %d", header->size);
			}
		}
	}
}


/*
** This function is only for debugging. It prints out the entire memory pool. 
** Use the code from the memdump tool to do this.
*/
//void ma_print(void)
//{
	//
//}
 
  

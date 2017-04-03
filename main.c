#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "ma_malloc.h"

int main( int argc, char *argv[] )
{
  char* ptr[] = {NULL,NULL,NULL};
  
  ma_init();
  assert( NULL == ma_malloc(600) ); //->should return NULL
  assert( NULL != ma_malloc(200) ); //->should not return NULL;
  
  ma_init();
  ptr[0] = ma_malloc(400); //->should not return NULL (ma_init() clears all)
  assert( ptr[0] != NULL );
  // for debugging: 
  memset(ptr[0], 'a', 400); //-> makes this area easy to find when using GDB or memdump-tool 
  ma_free(ptr[0]);
  
  ptr[0] = ma_malloc(200); //-> should not return NULL
  assert( ptr[0] != NULL );
  // for debugging: 
  memset(ptr[0], 'b', 200); //-> makes this area easy to find when using GDB or memdump-tool 

  ptr[1] = ma_malloc(100); //-> should not return NULL
  assert( ptr[1] != NULL );
  // for debugging: 
  memset(ptr[1], 'b', 100); //-> makes this area easy to find when using GDB or memdump-tool 

  ptr[2] = ma_malloc(100); //-> should not return NULL
  assert( ptr[2] != NULL );
  // for debugging: 
  memset(ptr[2], 'c', 100); //-> makes this area easy to find when using GDB or memdump-tool 
  
  ma_free(ptr[1]);
  ma_free(ptr[2]); //-> mind the sequence, first free 1 than 2
  assert( NULL != ma_malloc(250) ); //-> should not return NULL (test for coalescing)
  
  return 0;
}

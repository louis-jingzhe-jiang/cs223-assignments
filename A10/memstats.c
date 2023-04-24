#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "rand.h"

#define ROUNDS 3
#define BUFFER 5
#define LOOP 10

struct chunk {
  int size;
  int used;
  struct chunk *next;
};

void memstats(struct chunk* freelist, void* buffer[], int len) {
  int used = 0; // used block
  int freed = 0; // freed block
  int umem = 0; // used memory
  int fmem = 0; // freed memory
  int totmem = 0; // total memory
  struct chunk* this = freelist;
  for (int i = 0; i < len; i++) {
    if (buffer[i] != NULL) {
      printf("buffer[%i] is not NULL!\n", i);
      used ++;
      umem += ((struct chunk*) buffer[i] - 1)->used;
      totmem += ((struct chunk*) buffer[i] - 1)->size;
    }
  }
  while (this != NULL) {
    freed ++;
    fmem += this->size;
    totmem += this->size;
    this = this->next;
  }
  printf("Total blocks: %i  Free blocks: %i  Used blocks: %i\n", used + freed,
      freed, used);
  printf("Total memory allocated: %i  Free memory: %i Used memory: %i\n",
      totmem, fmem, umem);
  printf("Underutilized mdmory: %f\n", (float) fmem / totmem);
}

int main ( int argc, char* argv[]) {

  printf("Starting test..\n");

  srand(100);

  double timer;
  struct timeval tstart, tend;

  gettimeofday(&tstart, NULL);

  void *buffer[BUFFER];
  for (int i = 0; i < BUFFER; i++) {
    buffer[i] = NULL;
  }

  void *init = sbrk(0);
  void *current;
  printf("The initial top of the heap is %p.\n", init);
  for (int j = 0 ; j < ROUNDS; j++) {
    printf("---------------\n%d\n" , j);

    for (int i= 0 ; i < LOOP ; i++) {
      int index = rand() % BUFFER;
      if (buffer[index] != NULL) {
        free(buffer[index]);
        buffer[index] = NULL;
        printf("Freeing index %d\n", index);
      } 
      else {
        size_t size = (size_t) randExp(8, 4000); 
        int *memory = NULL;
        memory = malloc(size);

        if (memory == NULL) {
          fprintf(stderr, "malloc failed\n");
          return(1);
        } 
        *memory = 123;
        buffer[index] = memory;
        printf("Allocating %d bytes at index %d\n", (int) size, index);
      }
    }
    extern struct chunk* flist;
    current = sbrk(0);
    int allocated = current - init;
    init = current;

    printf("The new top of the heap is %p.\n", current);
    printf("Increased by %d (0x%x) bytes\n", allocated, allocated);
    memstats(flist, buffer, BUFFER);
  }

  for (int i = 0; i < BUFFER; i++) {
    free(buffer[i]); 
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
  printf("Time is %g\n", timer);

  return 0 ;
}

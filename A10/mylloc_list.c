#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct chunk {
  int size;
  int used;
  struct chunk *next;
};


struct chunk *flist = NULL;


void* malloc (size_t size) {
  if (size == 0) {
    return NULL;
  }
  struct chunk* next = flist;
  struct chunk* prev = NULL;
  while (next != NULL) {
    if (next->size >= size) { // this chunk has greater/equal size
      if (prev != NULL) { // not the first chunk in the list
        // remove the chunk from the list
        prev->next = next->next;
      } else { // the first chunk in the list
        // remove the first chunk
        flist = next->next;
      }
      next->used = size;
      return (void*) (next + 1); // return the chunk
    } else { // this chunk is smaller than size
      // go to next chunk
      prev = next;
      next = next->next;
    }
  }
  // if the program runs through here, no chunk has suitable size
  // we allocate new memory using sbrk()
  void* memory = sbrk(size + sizeof(struct chunk));
  if (memory == (void*) -1) {
    return NULL; // falied to sbrk
  } else {
    struct chunk* cnk = (struct chunk*) memory;
    cnk->size = size;
    cnk->used = size;
    cnk->next = NULL;
    return (void*) (cnk + 1);
  }
}


void free(void* memory) {
  if (memory != NULL) {
    struct chunk* cnk = (struct chunk*) ((struct chunk*) memory - 1);
    cnk->next = flist;
    flist = cnk;
    cnk->used = 0;
  }
  return;
}

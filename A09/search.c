/**
 * Author: Louis Jiang
 * Date: Apr 1, 2023
 * Description:
 * A program that uses N threads to search for a value in a list.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct target_range {
  int st;
  int ed;
  int target;
  int* arr;
  int t_num;
  int* found;
};

void* thread_search(void* args) {
  struct target_range* tr = (struct target_range*) args;
  for (int i = tr->st; i < tr->ed; i++) {
    if (tr->target == tr->arr[i]) {
      printf("Thread %i found %i at index %i\n", tr->t_num, tr->target,i);
      *tr->found = 1;
      break;
    }
  }
  pthread_exit(NULL);
  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage: %s <NumThreads>\n", argv[0]);
    exit(0);
  }
  int num_threads = atoi(argv[1]);
  // read in file
  FILE* fp = fopen("data.bin", "rb");
  int length; // the total number of integers in the data list
  fread(&length, sizeof(int), 1, fp);
  int* arr = malloc(sizeof(int) * length);
  fread(arr, sizeof(int), length, fp);
  fclose(fp);
  // user interface
  printf("Enter a value to search: ");
  int target;
  scanf(" %i", &target);
  // start searching
  int found = 0; // 0 for not found, 1 for found
  // generating threads
  pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
  // creating info structs
  struct target_range* tr = malloc(sizeof(struct target_range) * num_threads);
  int st = 0;
  int ed = length / num_threads;
  for (int i = 0; i < num_threads; i++) {
    if (i == num_threads - 1) {
      ed = length;
    }
    // assigning values into the info struct
    tr[i].st = st;
    tr[i].ed = ed;
    tr[i].target = target;
    tr[i].arr = arr;
    tr[i].t_num = i + 1;
    tr[i].found = &found;
    // spliting threads
    pthread_create(&threads[i], NULL, thread_search, &tr[i]);
    st += length / num_threads;
    ed += length / num_threads;
  }
  // joining the threads
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  if (found == 0) {
    printf("Not found!\n");
  }
  // freeing vars
  free(arr);
  free(threads);
  return 0;
}

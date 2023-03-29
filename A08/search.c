/**
 * Author: Louis Jiang
 * Date: Mar 28, 2023
 * Description:
 * A program that uses two processes to search for a value in a list.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Find an integer in an array of integers starting from index st and ends at 
 * index end (the index end is not included).
 * @param arr the pointer to the array of integers
 * @param target the integer we want to find
 * @param st the starting index
 * @param end the ending index (not included in search)
 * @return the index of the target in the array, -1 if not found
 */
int find(int* arr, int target, int st, int end) {
  for (int i = st; i < end; i++) {
    if (arr[i] == target) {
      return i;
    }
  }
  return -1;
}

int main() {
  // user interaction
  printf("Enter a value to search: ");
  int target;
  scanf(" %i", &target);
  // read the data file
  FILE* fp = fopen("data.bin", "rb");
  int size;
  fread(&size, sizeof(int), 1, fp); // read in the length of the list
  int* arr = malloc(sizeof(int) * size);
  fread(arr, sizeof(int), size, fp); // read in the data as a chunk
  fclose(fp);
  /* for DEBUG only
  for (int i = 0; i < size; i++) {
    printf("%i\n", arr[i]);
  } */
  // generating the parent and child processes
  pid_t pid = fork();
  if (pid) { // parent
    int result = find(arr, target, 0, size / 2);
    if (result != -1) {
      printf("Parent found %i at index %i\n", target, result);
      return 0;
    }
  } else { // child
    int result = find(arr, target, size / 2, size);
    if (result != -1) {
      printf("Child found %i at index %i\n", target, result);
      return 0;
    }
  }
  printf("Not found!\n");
  return 0;
}

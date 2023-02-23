/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 21, 2023
 * Description
 * A program that generates a crossword using two words from a user
 ---------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  // wrong user input
  if (argc < 3) {
    printf("Usage: %s <word1> <word2>\n", argv[0]);
    exit(0);
  }
  // main procedure
  char* firstWord = argv[1];
  char* secondWord = argv[2];
  // check whether there are any common letter and find one of them
  int i = 0; // 0 for no common letters, 1 for exist common letters
  int r; // the index of the common letter in firstWord
  int c; // the index of the common letter in secondWord
  for (int j = 0; j < strlen(firstWord); j++) {
    for (int k = 0; k < strlen(secondWord); k++) {
      if (firstWord[j] == secondWord[k]) {
        i++;
        r = j;
        c = k;
        break;
      }
    }
    if (i != 0) {
      break;
    }
  }
  if (i == 0) {
    printf("No common letter!\n");
  } else {
    char* cross = malloc(sizeof(char) * strlen(firstWord) * strlen(secondWord));
    for (int i = 0; i < strlen(firstWord) * strlen(secondWord); i++) {
      cross[i] = '.';
    }
    // common char in firstWord has index r
    // common char in secondWord has index c
    // let firstWord be column-alligned, secondWord be row-alligned
    // then firstWord locates in col c, and secondWord locates in row r
    for (int i = 0; i < strlen(firstWord); i++) {
      cross[i * strlen(secondWord) + c] = firstWord[i];
    }
    for (int i = 0; i < strlen(secondWord); i++) {
      cross[r * strlen(secondWord) + i] = secondWord[i];
    }
    // start printing out the result
    for (int i = 0; i < strlen(firstWord); i++) {
      for (int j = 0; j < strlen(secondWord); j++) {
        printf("%c ", cross[i * strlen(secondWord) + j]);
      }
      printf("\n");
    }
    free(cross);
  }
  return 0;
}

/***************************************************
 * mathgame.c
 * Author: Louis Jiang 
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
  int rounds = 0;
  int correct = 0;
  printf("Welcome to Math Game!\n");
  printf("How many rounds do you want to play? \n");
  scanf(" %d", &rounds);
  printf("\n");
  for (int i = 0; i < rounds; i++) {
    int a = rand() % 100;
    int b = rand() % 100;
    int sum = a + b;
    printf("\n%d + %d = ?\n", a, b);
    int ans = 0;
    scanf(" %d", &ans);
    if (ans == sum) {
      printf("\nCorrect :)\n");
      correct++;
    } else {
      printf("\nIncorrect :(\n");
    }
  }
  printf("\nYou answered %d/%d correctly.\n", correct, rounds);
  return 0;
}


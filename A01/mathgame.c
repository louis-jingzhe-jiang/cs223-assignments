/***************************************************
 * mathgame.c
 * Author: Louis Jiang 
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  time_t t;
  srand((unsigned) time(&t)); // initialize the random variable;
  int rounds = 0; // the number of rounds
  int correct = 0; // the number of correct answers
  printf("Welcome to Math Game!\n");
  printf("How many rounds do you want to play? \n");
  scanf(" %d", &rounds); // input the number of rounds
  printf("\n");
  for (int i = 0; i < rounds; i++) {
    int a = rand() % 100;
    int b = rand() % 100;
    int sum = a + b; // the right answer
    printf("\n%d + %d = ?\n", a, b);
    int ans = 0;
    scanf(" %d", &ans); // input the answer for the question
    if (ans == sum) { // compare the user's answer to the right answer
      printf("\nCorrect :)\n");
      correct++;
    } else {
      printf("\nIncorrect :(\n");
    }
  }
  printf("\nYou answered %d/%d correctly.\n", correct, rounds);
  return 0;
}


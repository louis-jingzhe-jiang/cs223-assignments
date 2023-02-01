/**
* A program that turns a word into an exclamation.
*
* The program will let the user input a word and then it randomly changes every
* lower case character in the word into one of '@', '!', '#', and '*'.
*
* @author: Louis Jiang
* @version: Jan 28, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  printf("Please enter a word: ");
  char word[32] = "";
  char specials[4] = "";
  specials[0] = '@';
  specials[1] = '!';
  specials[2] = '#';
  specials[3] = '*';
  scanf(" %s", word);
  int length = strlen(word);
  int i = 0;
  for (i = 0; i < length; i++) {
    if (word[i] > 96 && word[i] < 123) { // a lower case letter
      word[i] = specials[rand() % 4];
    }
  }
  printf("OMG! %s\n", word);
  return 0;
}

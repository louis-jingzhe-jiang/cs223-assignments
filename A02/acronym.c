/**
* A program that creates a string consisting of all capital letters from the
* given phrase.
*
* The program will ask the user to input a phrase and then it will display all
* the capital letters from the phrase onto the screen.
*
* @author: Louis Jiang
* @version: Jan 28, 2023
*/

#include <stdio.h>

int main() {
  printf("Please enter a phrase: ");
  char phrase[1024]; // the user-input phrase
  scanf("%[^\n]%*c", phrase);
  char acronym[1024]; // the acronym
  int i = 0; // the index of user-input phrase
  int j = 0; // the index of acronym
  while (phrase[i] != '\0') {
    if (phrase[i] > 64 && phrase[i] < 91) { // capital letter
      acronym[j] = phrase[i];
      j++;
    }
    i++;
  }
  acronym[j] = '\0'; // make sure that acronym is a string
  printf("Your acronym is %s\n", acronym);
  return 0;
}

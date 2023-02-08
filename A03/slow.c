/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 4, 2023
 * Description
 * A program that turns a phrase into an "ent-phrase"
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  printf("Pause length: ");
  int p_len; // the pause length
  scanf(" %i", &p_len);
  printf("Text: ");
  char text[32]; // the original text
  scanf(" %[^\n]%*c", text);
  char *ent_phrase;
  int text_len = strlen(text); // length of original text
  int ent_len = (p_len + 1) * text_len; // length of ent-phrase
  ent_phrase = malloc(sizeof(char) * (ent_len + 1)); // +1 due to null char
  //ent_phrase[0] = text[0];
  for (int i = 0; i < text_len; i++) {
    ent_phrase[i * (p_len + 1)] = text[i];
    for (int j = 1; j <= p_len; j++) {
      ent_phrase[i * (p_len + 1) + j] = '.';
    }
  }
  ent_phrase[ent_len] = '\0';
  printf("%s\n", ent_phrase);
  free(ent_phrase);
  return 0;
}

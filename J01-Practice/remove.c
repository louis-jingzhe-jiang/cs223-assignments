#include <stdio.h>
#include <string.h>

void remove1(char* str, char letter) {
  int length = strlen(str);
  char new_str[1024];
  int index = 0;
  for (int i = 0; i < length; i++) {
    if (str[i] != letter) {
      new_str[index] = str[i];
      index++;
    }
  }
  strcpy(str, new_str);
}

int main() {
  return 0;
}

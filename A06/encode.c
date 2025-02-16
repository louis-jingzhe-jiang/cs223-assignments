/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 26, 2023
 * Description
 * This program reads in a PPM file and asks the user for a message to embed
 * within it.
 ---------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: encode <file.ppm>\n");
    return 0;
  }
  // trying to read the ppm file
  int w;
  int h;
  struct ppm_pixel* pixels = read_ppm(argv[1], &w, &h);
  if (! pixels) { // file read error
    printf("The file %s can't be read or does not exist", argv[1]);
    return 0;
  }
  // file reading successful, display info
  printf("Reading %s with width %i and height %i\n", argv[1], w, h);
  int num_char = w * h * 3 / 8;
  printf("Max number of characters in the image: %i\n", num_char - 1);
  // allow user to input the phrase to be encoded
  printf("Enter a phrase: ");
  char* phrase = malloc(sizeof(char) * num_char);
  scanf(" %s", phrase);
  phrase[num_char - 1] = '\0'; // make sure the phrase ends with '\0'
  int phrlen = strlen(phrase); // the actual length of the phrase
  printf("If your phrase is longer than %i, ", num_char - 1);
  printf("only the first %i characters in your phrase ", num_char - 1);
  printf("will be encoded\n");
  // start to process the image to include the encoded message
  // for unsigned char* raw:
  // each char in the array stores only 1 bit of the data to be encoded
  unsigned char* raw = malloc(sizeof(unsigned char) * 8 * num_char);
  for (int i = 0; i < num_char; i++) {
    for (int j = 0; j < 8; j++) {
      // 
      raw[i * 8 + j] = (phrase[i] >> (7 - j)) & 1;
      // printf("%i", raw[i * 8 + j]); // DEBUG
    }
  }
  printf("\n");
  // only encode the data that is before the '\0' char
  for (int i = 0; i < (phrlen + 1) * 8; i++) {
    // printf("Before: %i %i %i\n", pixels[i / 3].red, pixels[i / 3].green,
    //     pixels[i / 3].blue); // DEBUG
    // printf("%i red:%i green:%i blue:%i\n", i / 3, pixels[i/3].red, 
    //    pixels[i/3].green, pixels[i/3].blue); // DEBUG
    if (i % 3 == 0) { // this bit should be stored in red channel
      if ((pixels[i / 3].red & 1) != raw[i]) { // not same bit, modify
        if (pixels[i / 3].red == 255) { // prevent overflow
          pixels[i / 3].red -= 1;
        } else {
          pixels[i / 3].red += 1;
        }
      }
    } else if (i % 3 == 1) { // this bit should be stored in green channel
      if ((pixels[i / 3].green & 1) != raw[i]) { // not same bit, modify
        if (pixels[i / 3].green == 255) { // prevent ovlerflow
          pixels[i / 3].green -= 1;
        } else {
          pixels[i / 3].green += 1;
        }  
      } 
    } else { // this bit should be stored in the blue channel
      if ((pixels[i / 3].blue & 1) != raw[i]) { // not same bit, modify
        if (pixels[i / 3].blue == 255) { // prevent overflow
          pixels[i / 3].blue -= 1;
        } else {
          pixels[i / 3].blue += 1;
        }  
      } 
    }
    // printf("After: %i %i %i\n", pixels[i / 3].red, pixels[i / 3].green,
    //     pixels[i / 3].blue); // DEBUG
  }
  // write out the new image
  char* new_filename = malloc(sizeof(char) * (strlen(argv[1]) + 9));
  int i = 0;
  for (i = 0; i < strlen(argv[1]) - 4; i++) {
    new_filename[i] = argv[1][i];
  }
  new_filename[i] = '\0';
  strcat(new_filename, "-encoded.ppm");
  printf("Writing file %s\n", new_filename);
  write_ppm(new_filename, pixels, w, h);
  // free the variables
  free(pixels);
  free(raw);
  free(new_filename);
  free(phrase);
  return 0;
}


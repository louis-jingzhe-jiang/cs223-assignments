/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 26, 2023
 * Description
 * A program that reads in a PPM file and then outputs any message that might
 * be stored in the least significant bits of each color. This program read
 * bits from each of the red, green, and blue colors -- top to bottom, left 
 * to right.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: decode <file.ppm>\n");
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
  printf("Reading %s with width %i and height %i\n", argv[1], w, h);
  int num_char = w * h * 3 / 8;
  printf("Max number of characters in the image: %i\n", num_char);
  // start decoding
  // each char needs to store only 1 bit of data here
  unsigned char* raw = malloc(sizeof(unsigned int) * w * h * 3);
  for (int i = 0; i < w * h; i++) {
    raw[i * 3] = pixels[i].red & 1;
    raw[i * 3 + 1] = pixels[i].green & 1;
    raw[i * 3 + 2] = pixels[i].blue & 1;
  }
  unsigned char* bytes = malloc(sizeof(char) * (num_char + 1));
  for (int i = 0; i < num_char; i++) {
    bytes[i] = 0; // initialize
    for (int j = 0; j < 8; j++) {
      bytes[i] = bytes[i] << 1;
      bytes[i] += raw[i * 8 + j];
    }
    if (bytes[i] == '\0') { // the end of the encoded message
      break;
    } else if (i == num_char - 1) { // message doesn't end with '\0', add '\0'
      bytes[i + 1] = '\0';
    }
  }
  printf("%s", bytes);
  free(pixels);
  free(raw);
  free(bytes);
  return 0;
}


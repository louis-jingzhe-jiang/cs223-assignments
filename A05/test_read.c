/*----------------------------------------------
 * Author: Louis Jiang 
 * Date: Feb 22, 2023
 * Description
 * A tester program to test the read_ppm function in read_ppm.c
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main() {
  int w, h;
  struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);
  printf("Testing file %s: %i %i\n", "feep-raw.ppm",  w, h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("(%i,%i,%i) ", pixels[i * w + j].red, pixels[i * w + j].green, 
          pixels[i * w + j].blue);
    }
    printf("\n");
  }
  // todo: print out the grid of pixels
  free(pixels);
  return 0;
}


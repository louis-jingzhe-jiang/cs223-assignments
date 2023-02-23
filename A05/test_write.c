/*----------------------------------------------
 * Author: Louis Jiang 
 * Date: Feb 22, 2023
 * Description
 * A program that is used to test the write_ppm function in write_ppm.c
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
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

  // test writing the file to test.ppm, reload it, and print the contents
  write_ppm("my.ppm", pixels, 4, 4);
  struct ppm_pixel* my = read_ppm("my.ppm", &w, &h);
  printf("Testing file %s: %i %i\n", "my.ppm",  w, h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("(%i,%i,%i) ", my[i * w + j].red, my[i * w + j].green,
          my[i * w + j].blue);
    }
    printf("\n");
  }
  free(pixels);
  free(my);
  return 0;
}

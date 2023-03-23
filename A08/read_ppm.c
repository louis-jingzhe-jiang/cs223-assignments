/*----------------------------------------------
 * Author: Louis Jiang 
 * Date: Feb 22, 2023
 * Description
 * A program that provides a function to read .ppm binary image files
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

// Choose *one* to implement (do not remove the other one!)

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  FILE* fp = fopen(filename, "rb");
  if (fp == NULL) { // file reading failed
    return NULL;
  }
  // read in the header
  // skip the useless info
  char ch[1024] = "";
  fscanf(fp, " %[^\n]%*c", ch);
  fscanf(fp, " %[^\n]%*c", ch);
  // start collecting the useful info
  int r; // the first number acquired
  int c; // the second number acquired
  fscanf(fp, " %d %d", &r, &c);
  // printf("%d %d\n", r, c); // display the result
  fscanf(fp, " %[^\n]%*c", ch); // this is the line of the max brightness
  struct ppm_pixel* result = malloc(sizeof(struct ppm_pixel*) * r * c);
  if (result == NULL) { // memory allocation failed
    return NULL;
  }
  // start reading the pixel data
  fread(result, sizeof(struct ppm_pixel), r * c, fp);
  // suppose width is the number of columns, and height is the number of rows
  *w = c;
  *h = r;
  fclose(fp);
  return result;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
  return NULL;
}

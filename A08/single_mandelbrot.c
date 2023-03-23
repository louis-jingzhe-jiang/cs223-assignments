/**
 * Author: Louis Jiang
 * Date: Mar 22, 2023
 * Description: A program that outputs a PPM image of the mandelbrot set
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"
#include "write_ppm.h"
#include <string.h>


/**
 * Find the index of the pixel in the array of pixels with its row number
 * and column number, given the number of columns of the array.
 * @param row the row number
 * @param col the column number
 * @param num_col the number of columns in the array
 * @return the index of the pixel
 */
int find_index(int row, int col, int num_col) {
  return row * num_col + col;
}


int main(int argc, char* argv[]) {
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>\n", argv[0]); break;
    }
  }
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // generate pallet
  srand(time(0));
  // allocate memory for the pixels of the image
  struct ppm_pixel* image = malloc(sizeof(struct ppm_pixel) * size * size);
  // generate color pallette p
  struct ppm_pixel* p = malloc(sizeof(struct ppm_pixel) * maxIterations);
  unsigned char baser = rand() % 255;
  unsigned char baseg = rand() % 255;
  unsigned char baseb = rand() % 255;
  for (int i = 0; i < maxIterations; i++) {
    p[i].red = baser + rand() % 100 - 50;
    p[i].green = baseg + rand() % 100 - 50;
    p[i].blue = baseb + rand() % 100 - 50;
  }
  // start timer
  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  // start calculating the colors
  int r = 0;
  int c = 0;
  for (r = 0; r < size; r++) {
    for (c = 0; c < size; c++) {
      float xfrac = r / (float) size;
      float yfrac = c / (float) size;
      float x0 = xmin + xfrac * (xmax - xmin);
      float y0 = ymin + yfrac * (ymax - ymin);
      float x = 0.0;
      float y = 0.0;
      int iter = 0;
      while (iter < maxIterations && x * x + y * y < 2 * 2) {
        float xtmp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
        iter++;
      }
      if (iter < maxIterations) { // diverge, paint color
        image[find_index(r, c ,size)].red = p[iter].red;
        image[find_index(r, c ,size)].green = p[iter].green;
        image[find_index(r, c ,size)].blue = p[iter].blue;
      } else { // converge, paint black
        image[find_index(r, c ,size)].red = 0;
        image[find_index(r, c ,size)].green = 0;
        image[find_index(r, c ,size)].blue = 0;
      }
    }
  }
  // end timer
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)
    / 1.e6;
  // display time used
  printf("Computed mandelbrot set (%ix%i) in %f seconds\n", 
      size, size, timer);
  // generate file name
  char filename[128] = "mandelbrot-";
  char size_str[16] = "";
  sprintf(size_str, "%d", size);
  strcat(filename, size_str);
  strcat(filename, "-");
  // generating time stamp
  int stamp = time(0);
  char time_stamp[16] = "";
  sprintf(time_stamp, "%d", stamp);
  strcat(filename, time_stamp);
  strcat(filename, ".ppm");
  // write file
  printf("Writing file: %s\n", filename);
  write_ppm(filename, image, size, size);
  // freeing variables
  free(image);
  free(p);
  return 0;
}

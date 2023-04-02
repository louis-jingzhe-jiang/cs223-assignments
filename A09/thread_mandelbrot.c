/**
 * Author: Louis Jiang
 * Date: Apr 1, 2023
 * Description:
 * A program that outputs a PPM image of the mandelbrot set using 
 * multi-threading
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"
#include <sys/time.h>

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

struct mand_data {
  struct ppm_pixel* image;
  int x_st;
  int x_ed;
  int y_st;
  int y_ed;
  int size;
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  int maxIterations;
  struct ppm_pixel* p;
  pthread_t* id;
};

/**
 * thread function
 */
void* thread_func(void* id) {
  struct mand_data* md = (struct mand_data*) id;
  printf("Thread # %lu sub-image block: cols (%i, %i) to rows (%i, %i)\n",
      *md->id, md->y_st, md->y_ed, md->x_st, md->x_ed);
  // start calculating the colors
  for (int r = md->y_st; r < md->y_ed; r++) {
    for (int c = md->x_st; c < md->x_ed; c++) {
      float xfrac = c / (float) md->size;
      float yfrac = r / (float) md->size;
      float x0 = md->xmin + xfrac * (md->xmax - md->xmin);
      float y0 = md->ymin + yfrac * (md->ymax - md->ymin);
      float x = 0.0;
      float y = 0.0;
      int iter = 0;
      while (iter < md->maxIterations && x * x + y * y < 2 * 2) {
        float xtmp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
        iter++;
      }
      if (iter < md->maxIterations) { // diverge, paint color
        md->image[find_index(r, c, md->size)].red = md->p[iter].red;
        md->image[find_index(r, c ,md->size)].green = md->p[iter].green;
        md->image[find_index(r, c ,md->size)].blue = md->p[iter].blue;
      } else { // converge, paint black
        md->image[find_index(r, c, md->size)].red = 0;
        md->image[find_index(r, c, md->size)].green = 0;
        md->image[find_index(r, c, md->size)].blue = 0;
      }
    }
  }
  printf("Thread # %lu finished\n", *md->id);
  return NULL;
}

int main(int argc, char* argv[]) {
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;
  int numProcesses = 4;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
    }
  }
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // set seed for the random number
  srand(time(0));
  // create image
  struct ppm_pixel* image = malloc(sizeof(struct ppm_pixel) * size * size);
  // generate pallet
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
  // compute image
  // spliting threads
  struct mand_data* md = malloc(sizeof(struct mand_data) * numProcesses);
  pthread_t* threads = malloc(sizeof(pthread_t) * numProcesses);
  for (int i = 0; i < numProcesses; i++) {
    md[i].image = image;
    md[i].size = size;
    md[i].xmin = xmin;
    md[i].xmax = xmax;
    md[i].ymin = ymin;
    md[i].ymax = ymax;
    md[i].maxIterations = maxIterations;
    md[i].p = p;
    md[i].id = &threads[i];
    if (i % 2 == 0) {
      md[i].x_st = 0;
      md[i].x_ed = size / 2;
    } else {
      md[i].x_st = size / 2;
      md[i].x_ed = size;
    }
    if (i - 2 < 0) {
      md[i].y_st = 0;
      md[i].y_ed = size / 2;
    } else {
      md[i].y_st = size / 2;
      md[i].y_ed = size;
    }
    pthread_create(&threads[i], NULL, thread_func, &md[i]);
  }
  // merging threads
  for (int i = 0; i < numProcesses; i++) {
    pthread_join(threads[i], NULL);
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
  free(p);
  free(image);
  free(threads);
  return 0;
}

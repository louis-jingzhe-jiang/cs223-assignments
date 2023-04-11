/**
 * Author: Louis Jiang
 * Date: Apr 8, 2023
 * Description:
 * A program that traces the trajectory of each point (x, y) as it escapes the
 * the mandelbrot set. This program uses multiple threads to achieve this goal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "read_ppm.h"
#include "write_ppm.h"


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


struct thread_data {
  int size;
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  int maxIterations;
  pthread_t* t_id;
  int x_st;
  int x_ed;
  int y_st;
  int y_ed;
};


/**
 * The mandelbrot set with only converge and diverge
 */
static unsigned char* mandelbrot;

/**
 * The final image of buddhabrot
 */
static struct ppm_pixel* image;

/**
 * The visited counts
 */
static int* counts;

/**
 * The max count
 */
static int maxCount;

/**
 * The barrier for threads
 */
pthread_barrier_t b;

/**
 * The mutual exclusion variables
 */
pthread_mutex_t mutex1, mutex2;


/**
 * thread function
 */
void* thread_func(void* id) {
  struct thread_data* d = (struct thread_data*) id;
  printf("Thread %lu sub-image block: cols (%i, %i) to rows (%i, %i)\n", 
      *d->t_id, d->x_st, d->x_ed, d->y_st, d->y_ed);
  // generate mandelbrot set
  for (int r = d->y_st; r < d->y_ed; r++) {
    for (int c = d->x_st; c < d->x_ed; c++) {
      float xfrac = c / (float) d->size;
      float yfrac = r / (float) d->size;
      float x0 = d->xmin + xfrac * (d->xmax - d->xmin);
      float y0 = d->ymin + yfrac * (d->ymax - d->ymin);
      float x = 0;
      float y = 0;
      int iter = 0;
      while (iter < d->maxIterations && x * x + y * y < 2 * 2) {
	float xtmp = x * x - y * y + x0;
	y = 2 * x * y + y0;
	x = xtmp;
	iter++;
      }
      if (iter < d->maxIterations) { // diverge
	// set membership at (r, c) to false
	mandelbrot[find_index(r, c, d->size)] = 0;
      } else { // converge
        // set membership at (r, c) to true
	mandelbrot[find_index(r, c, d->size)] = 1;
      }
    }
  }
  // compute visited counts
  int* this_count = malloc(sizeof(int) * d->size * d->size);
  // initialize
  for (int i = 0; i < d->size * d->size; i++) {
    this_count[i] = 0;
  }
  int this_max_count = 0;
  for (int r = d->y_st; r < d->y_ed; r++) {
    for (int c = d->x_st; c < d->x_ed; c++) {
      if (mandelbrot[find_index(r, c, d->size)] == 1) {
	continue;
      }
      float xfrac = c / (float) d->size;
      float yfrac = r / (float) d->size;
      float x0 = d->xmin + xfrac * (d->xmax - d->xmin);
      float y0 = d->ymin + yfrac * (d->ymax - d->ymin);
      float x = 0;
      float y = 0;
      while (x * x + y * y < 2 * 2) {
	float xtmp = x * x - y * y + x0;
	y = 2 * x * y + y0;
	x = xtmp;
	int yrow = (int) (d->size * (y - d->ymin) / (d->ymax - d->ymin));
	int xcol = (int) (d->size * (x - d->xmin) / (d->xmax - d->xmin));
	if (yrow < 0 || yrow >= d->size) { // out of range
	  continue;
	}
	if (xcol < 0 || xcol >= d->size) { // out of range
	  continue;
	}
	// increase count at (yrow, xcol)
	this_count[find_index(yrow, xcol, d->size)]++;
	// update max count
	if (this_count[find_index(yrow, xcol, d->size)] > this_max_count) {
	  this_max_count = this_count[find_index(yrow, xcol, d->size)];
	}
      }
    }
  }
  // update max count and count
  pthread_mutex_lock(&mutex1);
  for (int r = 0; r < d->size; r++) {
    for (int c = 0; c < d->size; c++) {
      counts[find_index(r,c,d->size)] += this_count[find_index(r,c,d->size)];
    }
  }
  if (maxCount < this_max_count) {
    maxCount = this_max_count;
  }
  pthread_mutex_unlock(&mutex1);
  // wait for all threads to finish till here
  pthread_barrier_wait(&b);
  free(this_count);
  // now go on to compute colors
  float gamma = 0.681;
  float factor = 1.0 / gamma;
  for (int r = d->y_st; r < d->y_ed; r++) {
    for (int c = d->x_st; c < d->x_ed; c++) {
      float value = 0;
      if (counts[find_index(r, c, d->size)] > 0) {
	value = log(counts[find_index(r, c, d->size)]) / log(maxCount);
	value = pow(value, factor);
      }
      int index = find_index(r, c, d->size);
      image[index].red = (int) (value * 255);
      image[index].green = (int) (value * 255);
      image[index].blue = (int) (value * 255);
    }
  }
  printf("Thread %lu finished\n", *d->t_id);
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

  // set seed for random number
  srand(time(0));
  // start timer
  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  // mandelbrot set
  mandelbrot = malloc(sizeof(unsigned char) * size * size);
  // buddhabrot image
  image = malloc(sizeof(struct ppm_pixel) * size * size);
  // count stats
  counts = malloc(sizeof(int) * size * size);
  for (int i = 0; i < size * size; i++) {
    counts[i] = 0;
  }
  // spliting threads
  pthread_t* threads = malloc(sizeof(pthread_t) * numProcesses);
  struct thread_data* d = malloc(sizeof(struct thread_data) * numProcesses);
  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);
  pthread_barrier_init(&b, NULL, 4);
  for (int i = 0; i < numProcesses; i++) {
    d[i].size = size;
    d[i].xmin = xmin;
    d[i].xmax = xmax;
    d[i].ymin = ymin;
    d[i].ymax = ymax;
    d[i].maxIterations = maxIterations;
    d[i].t_id = &threads[i];
    if (i % 2) {
      d[i].x_st = size / 2;
      d[i].x_ed = size;
    } else {
      d[i].x_st = 0;
      d[i].x_ed = size / 2;
    }
    if (i - 2 < 0) {
      d[i].y_st = 0;
      d[i].y_ed = size / 2;
    } else {
      d[i].y_st = size / 2;
      d[i].y_ed = size;
    }
    pthread_create(&threads[i], NULL, thread_func, &d[i]);
  }
  // merging threads
  for (int i = 0; i < numProcesses; i++) {
    pthread_join(threads[i], NULL);
  }
/*
  for (int i = 0; i < size * size; i++) {
    printf("%i  ", image[i].red);
    if (i % 480 == 479) {
      printf("end\n");
    }
  }
*/
  // end timer
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)
    / 1.e6;
  // display time used
  printf("Computed mandelbrot set (%ix%i) in %f seconds\n",
      size, size, timer);
  
  // generate file name
  char filename[128] = "buddhabrot-";
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

  // freeing vars
  free(mandelbrot);
  free(image);
  free(counts);
  free(threads);
  free(d);
  return 0;
}

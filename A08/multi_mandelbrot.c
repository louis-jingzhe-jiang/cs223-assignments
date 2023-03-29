#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
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
      case 'p': numProcesses = atoi(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
    }
  }
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // generate pallet
  srand(time(0));
  struct ppm_pixel* p = malloc(sizeof(struct ppm_pixel) * maxIterations);
  unsigned char baser = rand() % 255;
  unsigned char baseg = rand() % 255;
  unsigned char baseb = rand() % 255;
  for (int i = 0; i < maxIterations; i++) {
    p[i].red = baser + rand() % 100 - 50;
    p[i].green = baseg + rand() % 100 - 50;
    p[i].blue = baseb + rand() % 100 - 50;
  }
  // allocate shared memory for the pixels of the image
  int shmid1;
  shmid1 = shmget(IPC_PRIVATE, sizeof(struct ppm_pixel*) * size * size, 0644
      | IPC_CREAT);
  if (shmid1 == -1) {
    perror("Error: cannot initialize shared memory\n");
    free(p);
    exit(1);
  }
  struct ppm_pixel* image = shmat(shmid1, NULL, 0);
  if (image == (void*) -1) {
    perror("Error: cannot access shared memory\n");
    free(p);
    exit(1);
  }
  // start timer
  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  // compute image
  pid_t pid;
  int child_status;
  for (int i = 0; i < numProcesses; i++) {
    pid = fork();
    if (pid == 0) {
      printf("Launched child process %i\n", getpid());
      int rst = 0, cst = 0;
      int rend = size, cend = size;
      // determine the region of the graph
      if (i == 0) { // top left
        rend = size / 2;
        cend = size / 2;
      } else if (i == 1) { // top right
        cst = size / 2;
        rend = size / 2;
      } else if (i == 2) { // bottom left
        rst = size / 2;
        cend = size / 2;
      } else { // bottom right
        rst = size / 2;
        cst = size / 2;
      }
      printf("%i) Sub-image block: cols (%i, %i) to rows (%i, %i)\n",
            getpid(), cst, cend, rst, rend);
      // start the actual calculation
      for (int r = rst; r < rend; r++) {
        for (int c = cst; c < cend; c++) {
          float xfrac = c / (float) size;
          float yfrac = r / (float) size;
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
            image[find_index(r, c, size)].red = p[iter].red;
            image[find_index(r, c, size)].green = p[iter].green;
            image[find_index(r, c, size)].blue = p[iter].blue;
          }
        }
      }
      exit(0);
    }
  }
  for (int i = 0; i < numProcesses; i++) {
    pid_t ended = wait(&child_status);
    printf("Child process complete: %i\n", ended);
  }
  // end timer
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec) / 1.e6;
  // display time used
  printf("Computed mandelbrot set (%ix%i) in %f seconds\n", size, size, timer);
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
  // clean up shared memory
  if (shmdt(image) == -1) {
    perror("Error: cannot detatch from shared memory\n");
    free(p);
    exit(1);
  }
  if (shmctl(shmid1, IPC_RMID, 0) == -1) {
    perror("Error: cannot remove shared mdmory\n");
    free(p);
    exit(1);
  }
  // freeing pallet
  free(p);
}

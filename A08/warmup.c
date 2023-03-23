/**
 * Author: Louis Jiang
 * Date: Mar 21, 2023
 * Description: A program that generates 3 processes, each says Bye when the
 * process exits.
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // A0
  int current_pid = getpid();
  printf("%i  A0\n", current_pid);
  // B0 & B1
  pid_t processB = fork();
  if (processB) {
    current_pid = getpid();
    printf("%i  B0\n", current_pid);
    pid_t processC = fork();
    if (processC == 0) {
      current_pid = getpid();
      printf("%i  C1\n", current_pid);
    } else {
      current_pid = getpid();
      printf("%i  C0\n", current_pid);
    }
  } else {
    current_pid = getpid();
    printf("%i  B1\n", current_pid);
  }
  current_pid = getpid();
  printf("%i  Bye\n", current_pid);
  return 0;
}

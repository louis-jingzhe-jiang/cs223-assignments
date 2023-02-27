/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 25, 2023
 * Description
 * A program that reads in a single 64-bit unsigned integer and outputs it as
 * an 8x8 bit-per-pixel sprite.
 ---------------------------------------------*/
#include <stdio.h>

int main() {
  unsigned long img;
  scanf(" %lx", &img);
  printf("Image (unsigned long): %lx\n", img);
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      unsigned long mask = 0x1ul << (63 - (r * 8 + c));
      int val = (img & mask) >> (63 - (r * 8 + c));
      if (val) {
        printf("@ ");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }
  return 0;
}

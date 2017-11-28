/* 
 * io.c
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * Implementation of the input and output functions
 */

#include <inttypes.h>
#include "io.h"

uint32_t input() {
        char c = getchar();
        if (c == EOF) {
                return -1;
        } else {
                return c;
        }
}

void output(uint32_t display) {
        putchar(display);
}
/* 
 * io.h
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * An interface that performs the instructions that read in from standard input
 * and print to standard output. 
 */

#ifndef IO_INCLUDED
#define IO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uarray.h"

uint32_t input();
void output(uint32_t display);

#endif
/* 
 * um.h
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * An interface that reads in and runs all instructions.
 */

#ifndef UM_INCLUDED
#define UM_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <fcntl.h>
#include "bitpack.h"
#include "segments.h"
#include "operations.h"
#include "io.h"
#include "uarray.h"
#include "bitpack.h"


Segments *read_instructions(FILE *fp, char *filename);
void run_instructions(Segments *s);
void load_value(uint32_t curr_instruction, UArray_T registers);
void load_program(Segments *s, uint32_t b, uint32_t c);

#endif
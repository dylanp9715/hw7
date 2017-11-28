/* 
 * operations.h
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * An interface that handles all the basic operations performed on values 
 * within registers, halting the program, and placing the value of special 
 * instruction in register A.
 */

#ifndef OPERATIONS_INCLUDED
#define OPERATIONS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uarray.h"

uint32_t conditional_move(uint32_t a, uint32_t b, uint32_t c);
uint32_t add(uint32_t b, uint32_t c);
uint32_t multiply(uint32_t b, uint32_t c);
uint32_t divide(uint32_t b, uint32_t c);
uint32_t nand(uint32_t b, uint32_t c);
// void halt();
// void load_program(UArray_T registers, UArray_T segment_zero, 
//                   uint32_t *program_counter);
// void load_value(UArray_T registers, unsigned value);

#endif
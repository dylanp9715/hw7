/* 
 * segments.h
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * An interface that handles all segment related instructions.
 */

#ifndef SEGMENTS_INCLUDED
#define SEGMENTS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include "uarray.h"
#include "seq.h"

// typedef struct {
//         Seq_T openID;
//         Seq_T segments;
// } Segments;

Seq_T openID;

uint32_t map_segment(uint32_t **segments, int num_words, int seg_size);
void unmap_segment(uint32_t **segments, int unmapID);
void segment_store(uint32_t **segments, int segmentID, int offset, uint32_t word);
uint32_t segment_load(uint32_t **segments, int segmentID, int offset);
void free_all_segments(uint32_t **segments, int seg_capacity);
int get_size(int index);
void set_size(int index, int size);

#endif
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
#include "uarray.h"
#include "seq.h"

typedef struct {
        Seq_T openID;
        Seq_T segments;
} Segments;


uint32_t map_segment(Segments *s, int num_words);
void unmap_segment(Segments *s, int unmapID);
void segment_store(Segments *s, int segmentID, int offset, uint32_t word);
uint32_t segment_load(Segments *s, int segmentID, int offset);
void free_all_segments(Segments *s);

#endif
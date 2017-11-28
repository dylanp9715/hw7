/* 
 * segments.c
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * The implementation of the segments interface which handles all segmnet 
 * related instructions
 */

#include <inttypes.h>
#include "segments.h"

void check_openID(Segments *s);
void initialize_segments(Segments *s);
int next_open_ID(Segments *s);
void unmap_all_segments(Segments *s);

/* Purpose: Maps a segment to the next open spot in memory with a size of given
 * number of words
 * Arguments: Takes in a Segments Struct with segments and next open id's, as 
 * well as the number of words that the mapped segment should be able to hold
 * Returns: the segment ID
 */
uint32_t map_segment(Segments *s, int num_words) {
        check_openID(s);
        int new_ID = 0;

        if (Seq_length(s->openID) == 0) {
                new_ID = Seq_length(s->segments);
                Seq_addhi(s->segments, UArray_new(num_words, 
                                                  sizeof(uint32_t)));
                return new_ID;
        }
        else {
                int *reused_ID = (int *)Seq_remlo(s->openID);
                Seq_put(s->segments, *reused_ID, UArray_new(num_words, 
                         sizeof(uint32_t)));
                new_ID = *reused_ID;
                free(reused_ID);
                return new_ID;
        }
}

/* Purpose: Checks to see if the openID's sequence has been initialized yet
 * Arguments: Takes in a Segments Struct with segments and next open id's
 * Returns: Nothing it is a void function
 */
void check_openID(Segments *s) {
        if (s->openID == NULL) {
                initialize_segments(s);
        }
}

/* Purpose: Initializes a segment struct each sequence getting size 10
 * Arguments: Takes in a Segments Struct with segments and next open id's
 * Returns: Nothing it is a void function
 */

void initialize_segments(Segments *s) {
        s->openID = Seq_new(10);
        s->segments = Seq_new(10);
}

/* Purpose: Unmaps a previously mapped segment 
 * Arguments: Takes in a Segments Struct with segments and next open id's and
 * the ID to a previously mapped segment
 * Returns: Nothing it is a void function
 */
void unmap_segment(Segments *s, int unmapID) {
        int *new_ID = malloc(sizeof(int));
        UArray_T temp = (UArray_T)Seq_get(s->segments, unmapID);
        UArray_free(&temp);
        Seq_put(s->segments, unmapID, NULL);
        *new_ID = unmapID;
        Seq_addhi(s->openID, new_ID);
}

/* Purpose: Stores a word in a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID
 * an offset, and a word
 * Returns: Nothing it is a void function
 */
void segment_store(Segments *s, int segmentID, int offset, uint32_t word) {

        UArray_T temp = (UArray_T)Seq_get(s->segments, segmentID);
        *(uint32_t *)UArray_at(temp, offset) = word;
}

/* Purpose: Loads a word from a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID, and 
 * offset
 * Returns: The word at the segments given offset to be loaded into a register
 */
uint32_t segment_load(Segments *s, int segmentID, int offset) {
        UArray_T temp = Seq_get(s->segments, segmentID);
        uint32_t word = *(uint32_t *)UArray_at(temp, offset);
        return word;
}

/* Purpose: Free all the segments in the Segments struct as well as all the 
 * openID's that were malloc'd
 * Arguments: Segments Struct with segments
 * Returns: Nothing it is a void function
 */
void free_all_segments(Segments *s) {

        int seg_length = Seq_length(s->segments);
        int id_length = Seq_length(s->openID);

        for (int i = 0; i < seg_length; ++i) {

                if (Seq_get(s->segments, i) != NULL){

                        UArray_T temp = (UArray_T)Seq_get(s->segments, i);
                        UArray_free(&temp);

                }

        }
        for (int j = 0; j < id_length; j++){
                int *temp = Seq_get(s->openID, j);

                free(temp);
        }
        Seq_free(&(s->openID));
        Seq_free(&(s->segments));
}
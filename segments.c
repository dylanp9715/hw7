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
//int next_open_ID(Segments *s);
void unmap_all_segments(Segments *s);
//void expand(Segments *s);
//int seg_size = 0;
//int seg_capacity = 5;

/* Purpose: Maps a segment to the next open spot in memory with a size of given
 * number of words
 * Arguments: Takes in a Segments Struct with segments and next open id's, as 
 * well as the number of words that the mapped segment should be able to hold
 * Returns: the segment ID
 */
uint32_t map_segment(Segments *s, int num_words) {
        check_openID(s);
        int new_ID = 0;
        //printf("Num words: %d\n", num_words);
        // if (seg_size >= seg_capacity - 1){
        //         fprintf(stderr, "IN EXPAND\n");
        //         expand(&segments);
        // }
        //printf("seg_size: %d\n", seg_size);
        if (Seq_length(s->openID) == 0) {
                new_ID = Seq_length(s->segments);
                //new_ID = seg_size;
                //printf("here\n");
                uint32_t *segment = calloc((num_words + 1), sizeof(uint32_t *));
                segment[0] = num_words;
                //}
                //else {
                //        segment = malloc(sizeof(uint32_t) * (num_words));
                //}
                //printf("new_ID: %d\n", new_ID);
                //fprintf(stderr, "NewID = %d segments size = %d\n", new_ID, seg_size);
                //segments[new_ID] = segment;
                Seq_addhi(s->segments, segment);
                //seg_size++;
                return new_ID;
        }
        else {
                int *reused_ID = (int *)Seq_remlo(s->openID);
                uint32_t *segment = calloc((num_words + 1), sizeof(uint32_t *));
                segment[0] = num_words;
                //}
                //else {
                //        segment = malloc(sizeof(uint32_t) * (num_words));
                // }
                Seq_put(s->segments, *reused_ID, segment);
                //segments[*reused_ID] = segment;
                new_ID = *reused_ID;
                free(reused_ID);
                //seg_size++;
                return new_ID;
        }
}

//void expand(uint32_t ***segments){
//        *segments = realloc(*segments, (seg_capacity * 2 + 1) * sizeof(uint32_t *));
        /*uint32_t **new_array = calloc((seg_capacity * 2 + 1), sizeof(uint32_t *));
        for (int i = 0; i < seg_size; ++i){
                new_array[i] = *segments[i];
        }
        free(*segments);
        *segments = new_array;*/
//        seg_capacity = (seg_capacity * 2) + 1;
//}

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
        //printf("here2\n");
        int *new_ID = malloc(sizeof(int));
        //UArray_T temp = (UArray_T)Seq_get(s->segments, unmapID);
        //UArray_free(&temp);
        uint32_t *temp = (uint32_t *)Seq_get(s->segments, unmapID);
        //uint32_t *temp = segments[unmapID];
        free(temp);
        Seq_put(s->segments, unmapID, NULL);
        *new_ID = unmapID;
        Seq_addhi(s->openID, new_ID);
        //seg_size--;
        //printf("seg_size: %d\n", seg_size);
}

/* Purpose: Stores a word in a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID
 * an offset, and a word
 * Returns: Nothing it is a void function
 */
void segment_store(Segments *s, int segmentID, int offset, uint32_t word) {

        //UArray_T temp = (UArray_T)Seq_get(s->segments, segmentID);
        uint32_t *temp = (uint32_t *)Seq_get(s->segments, segmentID);
        //fprintf(stderr, "SEGMENT ID IS = %d\n", segmentID); 
        //uint32_t *temp = segments[segmentID];
        //*(uint32_t *)UArray_at(temp, offset) = word;
        //if (segmentID != 0) {
        //printf("offset, word: %d, %x\n", offset, word);
        //printf("here\n");
        temp[offset] = word;


        //} else {
                //printf("offset, word: %d, %d\n", offset, word);
                //temp[offset] = word;
        //}
}

/* Purpose: Loads a word from a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID, and 
 * offset
 * Returns: The word at the segments given offset to be loaded into a register
 */
uint32_t segment_load(Segments *s, int segmentID, int offset) {
        //UArray_T temp = Seq_get(s->segments, segmentID);
        uint32_t *temp = (uint32_t *)Seq_get(s->segments, segmentID);
        //printf("segID: %d\n", word);
        //uint32_t *temp = segments[segmentID];
        //uint32_t word;
        //if (segmentID != 0) {
                //printf("here\n");
        uint32_t word = temp[offset];
        //printf("seg_size: %d\n", seg_size);
        //} else {
        //        word = temp[offset];
        //}
        return word;
}

/* Purpose: Free all the segments in the Segments struct as well as all the 
 * openID's that were malloc'd
 * Arguments: Segments Struct with segments
 * Returns: Nothing it is a void function
 */
void free_all_segments(Segments *s) {

        int seg_length = Seq_length(s->segments);//seg_size;//Seq_length(s->segments);
        int id_length = Seq_length(s->openID);

        for (int i = 0; i < seg_length; ++i) {

                if (Seq_get(s->segments, i) != NULL){

                        //UArray_T temp = (UArray_T)Seq_get(s->segments, i);
                        uint32_t *temp = (uint32_t *)Seq_get(s->segments, i);//segments[i];//(uint32_t *)Seq_get(s->segments, i);
                        //UArray_free(&temp);
                        free(temp);

                }

        }
        for (int j = 0; j < id_length; j++){
                int *temp = Seq_get(s->openID, j);
                free(temp);
        }
        Seq_free(&(s->openID));
        Seq_free(&(s->segments));
        //free(segments);
}

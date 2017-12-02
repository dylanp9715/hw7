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

void check_openID();
void unmap_all_segments(uint32_t **segments);
//uint32_t **expand(uint32_t **segments);
//int seg_size = 0;
//int seg_capacity = 50000;

/* Purpose: Maps a segment to the next open spot in memory with a size of given
 * number of words
 * Arguments: Takes in a Segments Struct with segments and next open id's, as 
 * well as the number of words that the mapped segment should be able to hold
 * Returns: the segment ID
 */
uint32_t map_segment(uint32_t **segments, int num_words, int seg_size) {
        check_openID();
        int new_ID = 0;
        //uint32_t **segments = *old_segments;
        // if (seg_size >= seg_capacity){
        //         segments = expand(segments);
        //         //fprintf(stderr, "new_array num_words: %d\n", segments[0][0]);
        // }
        if (Seq_length(openID) == 0) {
                new_ID = seg_size;
                uint32_t *segment = calloc((num_words + 1), sizeof(uint32_t *));
                segment[0] = num_words;
                //fprintf(stderr, "here\n");
                segments[new_ID] = segment;
                //fprintf(stderr, "made it out\n");
                //printf("new_array num_words: %d\n", segments[0][0]);
                return new_ID;
        }
        else {
                int *reused_ID = (int *)Seq_remlo(openID);
                uint32_t *segment = calloc((num_words + 1), sizeof(uint32_t *));
                segment[0] = num_words;
                segments[*reused_ID] = segment;
                new_ID = *reused_ID;
                free(reused_ID);
                return new_ID;
        }
}

// uint32_t **expand(uint32_t **segments){
//         //segments = realloc(segments, (seg_capacity * 2 + 1) * sizeof(uint32_t *));
//         //fprintf(stderr, "expand\n");
//         //fprintf(stderr, "expanding: %d\n", *segments[0][0]);
//         uint32_t **new_array = calloc((seg_capacity * 2 + 1), sizeof(uint32_t *));
//         for (int i = 0; i < seg_capacity; ++i){
//                 // printf("seg_capacity: %d\n", seg_capacity);
//                 new_array[i] = segments[i];
//                 // printf("new_array num_words: %d\n", new_array[i][0]);
//                 // printf("i: %d\n", i);
//         }
//         free(segments);
//         segments = new_array;
//         seg_capacity = (seg_capacity * 2) + 1;
//         // for (int j = 0; j < 50; ++j) {
//         //         printf("%d\n", segments[j][0]);
//         // }
//         //printf("here\n");
//         return segments;
//         //uint32_t foo = *segments[0][0];
//         //fprintf(stderr, "size of seg 0 at resize: %d\n", foo);
//         // fprintf(stderr, "resizing\n");
//         //(void) foo;
// }

/* Purpose: Checks to see if the openID's sequence has been initialized yet
 * Arguments: Takes in a Segments Struct with segments and next open id's
 * Returns: Nothing it is a void function
 */
void check_openID() {
        if (openID == NULL) {
                openID = Seq_new(10);
        }
}

/* Purpose: Unmaps a previously mapped segment 
 * Arguments: Takes in a Segments Struct with segments and next open id's and
 * the ID to a previously mapped segment
 * Returns: Nothing it is a void function
 */
void unmap_segment(uint32_t **segments, int unmapID) {
        int *new_ID = malloc(sizeof(int));
        free(&segments[unmapID][0]);
        segments[unmapID] = NULL;
        *new_ID = unmapID;
        Seq_addhi(openID, new_ID);
        //seg_size--;
}

/* Purpose: Stores a word in a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID
 * an offset, and a word
 * Returns: Nothing it is a void function
 */
void segment_store(uint32_t **segments, int segmentID, int offset, uint32_t word) {
        segments[segmentID][offset] = word;
}

/* Purpose: Loads a word from a specified segment at a given offset
 * Arguments: Takes in a Segments Struct with segments, a segment ID, and 
 * offset
 * Returns: The word at the segments given offset to be loaded into a register
 */
uint32_t segment_load(uint32_t **segments, int segmentID, int offset) {
        return segments[segmentID][offset];
}

/* Purpose: Free all the segments in the Segments struct as well as all the 
 * openID's that were malloc'd
 * Arguments: Segments Struct with segments
 * Returns: Nothing it is a void function
 */
void free_all_segments(uint32_t **segments, int seg_capacity) {

        int seg_length = seg_capacity;
        int id_length = Seq_length(openID);

        for (int i = 0; i < seg_length; ++i) {

                if (segments[i] != NULL){
                        free(&segments[i][0]);

                }

        }
        for (int j = 0; j < id_length; j++){
                int *temp = Seq_get(openID, j);
                free(temp);
        }
        Seq_free(&openID);
}

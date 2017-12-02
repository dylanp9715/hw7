/* 
 * instruction_handler.c
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * The implementation of the instruction_handler interface which reads in and 
 * runs all instructions.
 */

#include <assert.h>
#include "instruction_handler.h"

uint32_t registers[8];
int seg_size = 0;
int seg_capacity = 50;
int seg_zero_size = 0;
uint32_t **segments = NULL;

void store_instruction(uint32_t *seg_zero, uint32_t instruction, int open_index);
uint32_t unpack_opcode(uint32_t instruction);
int call_instruction(uint32_t opcode, 
                      uint32_t curr_instruction);
void perform_op(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode);
void perform_seg(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode);
void perform_io(uint32_t value, uint32_t opcode, uint32_t c);
void unpack_one_register(uint32_t curr_instruction);
void expand();

/* Purpose: Reads the instructions and stores them into segment zero
 * Arguments: File pointer and the filename
 * Returns: Updated pointer to the Segments struct
 */
uint32_t **read_instructions(FILE *fp, char *filename) {
        int open_index = 1;
        segments = calloc(seg_capacity, sizeof(uint32_t *));
        openID = NULL;

        int file = 0;
        if((file=open(filename,O_RDONLY)) < -1)
                return NULL;
 
        struct stat fileStat;
        if(fstat(file, &fileStat) < 0)    
                return NULL;

        int num_words = fileStat.st_size / 4;
        map_segment(segments, num_words, seg_size);
        seg_size++;
        //printf("here\n");
        uint32_t *seg_zero = segments[0];
        for (int i = 0; i < num_words; ++i) {
                uint32_t word = 0;
                for (int j = 0; j < 4; j++) {
                        int c = fgetc(fp);
                        word = Bitpack_newu(word, 8, 24 - (8 * j), c);
                }
                store_instruction(seg_zero, word, open_index);
                open_index++;
        }

        fclose(fp);
        return segments;
}
void expand(){
        //segments = realloc(segments, (seg_capacity * 2 + 1) * sizeof(uint32_t *));
        //fprintf(stderr, "expand\n");
        //fprintf(stderr, "expanding: %d\n", *segments[0][0]);
        uint32_t **new_array = calloc((seg_capacity * 2 + 1), sizeof(uint32_t *));
        for (int i = 0; i < seg_capacity; ++i){
                //printf("seg_capacity: %d\n", seg_capacity);
                new_array[i] = segments[i];
                //printf("new_array num_words: %d\n", new_array[i][0]);
                // printf("i: %d\n", i);
        }
        free(segments);
        segments = new_array;
        seg_capacity = (seg_capacity * 2) + 1;
        //return segments;
        // for (int j = 0; j < 50; ++j) {
        //         printf("%d\n", segments[j][0]);
        // }
        //printf("here\n");
        //return segments;
        //uint32_t foo = *segments[0][0];
        //fprintf(stderr, "size of seg 0 at resize: %d\n", foo);
        // fprintf(stderr, "resizing\n");
        //(void) foo;
}
/* Purpose: Places instructions into segment zero
 * Arguments: Segment zero, the instruction, and the next open index to place 
 * the instruction into
 * Returns: void
 */
void store_instruction(uint32_t *seg_zero, uint32_t instruction, 
                       int open_index) {
        seg_zero[open_index] = instruction;
}

/* Purpose: Creates the registers array, updates the program counter, and 
 * goes through segment zero to call the function "call instructions" on each.
 * Arguments: Segments pointer
 * Returns: void
 */
void run_instructions() {
        int instruction_counter = 1;
        uint32_t *seg_zero = segments[0];
        seg_zero_size = seg_zero[0];
        
        while (instruction_counter < seg_zero_size + 1) {
                uint32_t curr_instruction = seg_zero[instruction_counter];
                uint32_t opcode = unpack_opcode(curr_instruction);
                int new_counter = call_instruction(opcode, 
                                                   curr_instruction);
                if (new_counter != -1) {
                        instruction_counter = new_counter;
                        seg_zero = segments[0];
                }
                else {
                        instruction_counter++;
                }
        }
}

/* Purpose: Unpacks the opcode from the passed-in instruction
 * Arguments: The instruction
 * Returns: The opcode
 */
uint32_t unpack_opcode(uint32_t instruction) {
        uint32_t opcode = instruction >> 28;
        //printf("opcode: %d\n", opcode);
        return opcode;
}

/* Purpose: Performs the load value instruction
 * Arguments: The special instruction and the registers
 * Returns: void
 */
void load_value(uint32_t curr_instruction) {
        int num_regi = curr_instruction << 4 >> 29;
        registers[num_regi] = curr_instruction << 7 >> 7;
}

/* Purpose: Calls certain functions depending on the opcode that is passed in
 * Arguments: Opcode, registers, Segments struct pointer, and the current
 * instruction
 * Returns: Returns -1 if load program was not called and the value in register
 * c otherwise so that run_instructions can properly assign the program counter
 */
int call_instruction(uint32_t opcode, 
                     uint32_t curr_instruction) {
        int c = -1;
        if (opcode == 13) {
                load_value(curr_instruction);
        }

        uint32_t ra = curr_instruction << 23 >> 29;
        uint32_t rb = curr_instruction << 26 >> 29;
        uint32_t rc = curr_instruction << 29 >> 29;

        if (opcode == 0 || (2 < opcode && opcode < 7)) {
                perform_op(ra, rb, rc, opcode);
        }
        if ((0 < opcode && opcode < 3) || (7 < opcode && opcode < 10)) {
                perform_seg(ra, rb, rc, opcode);
        }
        if (opcode == 7) {
                free_all_segments(segments, seg_capacity);
                free(segments);
                exit(0);
        }
        if (opcode == 10 || opcode == 11) {
                uint32_t value_c = registers[rc];
                perform_io(value_c, opcode, rc);
        }
        if (opcode == 12) {
                uint32_t value_b = registers[rb];
                uint32_t value_c = registers[rc];
                load_program(value_b, value_c);
                c = value_c + 1;
        }
        return c;
}

/* Purpose: Calls the necessary operations function based on the opcode
 * Arguments: Registers and the number register for a, b, and c
 * Returns: void
 */
void perform_op(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode) {
        uint32_t value_a = registers[a];
        uint32_t value_b = registers[b];
        uint32_t value_c = registers[c];
        if (opcode == 0) {
               registers[a] = conditional_move(value_a, value_b, value_c);
        }
        else if (opcode == 3) {
                uint32_t sum = add(value_b, value_c);
                registers[a] = sum;
        }
        else if (opcode == 4) {
                uint32_t result = multiply(value_b, value_c);
                registers[a] = result;
        }
        else if (opcode == 5) {
                uint32_t result = divide(value_b, value_c);
                registers[a] = result;
        }
        else {
                uint32_t result = nand(value_b, value_c);
                registers[a] = result;
        }
}

/* Purpose: Calls the necessary segments function based on the opcode
 * Arguments: Registers and the number register for a, b, and c
 * Returns: void
 */
void perform_seg(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode) {
        uint32_t value_a = registers[a];
        uint32_t value_b = registers[b];
        uint32_t value_c = registers[c];
        if (opcode == 1) {
                registers[a] = segment_load(segments, value_b , value_c + 1); 
        }
        else if (opcode == 2) {
                uint32_t segmentID = value_a;
                segment_store(segments, segmentID, value_b + 1, value_c);
                //fprintf(stderr, "stored val: %x\n", segments[segmentID][value_b+1]);

        }
        else if (opcode == 8) {
                if (seg_size >= seg_capacity){
                        //printf("here\n");
                        expand(segments);
                        //fprintf(stderr, "we out\n");
                //fprintf(stderr, "new_array num_words: %d\n", segments[0][0]);
                }
                registers[b] = map_segment(segments, value_c, seg_size);
                seg_size++;
        }
        else {
                unmap_segment(segments, value_c);
                seg_size--;
        }
}

/* Purpose: Calls the necessary io function based on the opcode
 * Arguments: Value to output and opcode
 * Returns: void
 */
void perform_io(uint32_t value, uint32_t opcode, uint32_t c) {
        uint32_t output_val = value;
        if (opcode == 10) {
                output(output_val);
        } else {
                registers[c] = input();
        }
}

/* Purpose: Performs the load program instruction
 * Arguments: Segments struct pointer and the values in register b and c
 * Returns: void
 */
void load_program(uint32_t b, uint32_t c) {
        (void) c;
        if (b != 0) {
                uint32_t *seg_zero = segments[0];
                free(seg_zero);
                uint32_t *temp = segments[b];
                uint32_t *copy_seg = calloc((temp[0]+1), sizeof(uint32_t));
                int temp_size = temp[0];

                for (int i = 0; i < temp_size + 1; i++){
                        copy_seg[i] = temp[i];
                }

                seg_zero_size = temp[0];
                segments[0] = copy_seg;
        }
}
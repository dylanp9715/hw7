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

void store_instruction(uint32_t *seg_zero, uint32_t instruction, int open_index);
uint32_t unpack_opcode(uint32_t instruction);
int call_instruction(uint32_t opcode, Segments *s, 
                      uint32_t curr_instruction);
void perform_op(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode);
void perform_seg(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode, Segments *s);
void perform_io(uint32_t value, uint32_t opcode);
void unpack_one_register(uint32_t curr_instruction);

/* Purpose: Reads the instructions and stores them into segment zero
 * Arguments: File pointer and the filename
 * Returns: Updated pointer to the Segments struct
 */
Segments *read_instructions(FILE *fp, char *filename) {
        int open_index = 1;
        Segments *s = malloc(sizeof(Segments));
        s->openID = NULL;
        s->segments = NULL;

        int file = 0;
        if((file=open(filename,O_RDONLY)) < -1)
                return NULL;
 
        struct stat fileStat;
        if(fstat(file, &fileStat) < 0)    
                return NULL;

        int num_words = fileStat.st_size / 4;
        map_segment(s, num_words);
        //UArray_T seg_zero = (UArray_T) Seq_get(s->segments, 0);
        uint32_t *seg_zero = (uint32_t *)Seq_get(s->segments, 0);
        for (int i = 0; i < num_words; ++i) {
                uint32_t word = 0;
                for (int j = 0; j < 4; j++) {
                        int c = fgetc(fp);
                        word = Bitpack_newu(word, 8, 24 - (8 * j), c);
                }
                store_instruction(seg_zero, word, open_index);
                //printf("curr_instruction1: %x\n", seg_zero[i]);
                open_index++;
        }
        //s->size_zero = num_words;
        fclose(fp);
        return s;
}

/* Purpose: Places instructions into segment zero
 * Arguments: Segment zero, the instruction, and the next open index to place 
 * the instruction into
 * Returns: void
 */
void store_instruction(uint32_t *seg_zero, uint32_t instruction, 
                       int open_index) {
        //*(uint32_t *) UArray_at(seg_zero, open_index) = instruction;
        seg_zero[open_index] = instruction;
}

/* Purpose: Creates the registers array, updates the program counter, and 
 * goes through segment zero to call the function "call instructions" on each.
 * Arguments: Segments pointer
 * Returns: void
 */
void run_instructions(Segments *s) {
        //UArray_T registers = UArray_new(8, sizeof(uint32_t));
        int instruction_counter = 1;
        //printf("here\n");
        //UArray_T seg_zero = Seq_get(s->segments, 0);

        uint32_t *seg_zero = Seq_get(s->segments, 0);
        seg_size = seg_zero[0];//s->size_zero;
        //printf("here1\n");
        //uint32_t *program_counter = (uint32_t *) UArray_at(seg_zero, 
        //                                             instruction_counter);
        //uint32_t *program_counter = (seg_zero[instruction_counter]);
        //printf("here2\n");
        //printf("program_counter: %x\n", *program_counter);
        //printf("seg_size: %d\n", seg_size);
        while (instruction_counter < seg_size + 1) {
                uint32_t curr_instruction = seg_zero[instruction_counter];
                //printf("curr_instruction2: %x\n", seg_zero[instruction_counter]);
                uint32_t opcode = unpack_opcode(curr_instruction);
                int new_counter = call_instruction(opcode, s, 
                                                   curr_instruction);
                //printf("new_counter: %d\n", new_counter);
                //int new_counter = -1;
                //printf("instruction_counter f: %d\n", instruction_counter);
                if (new_counter != -1) {
                        //printf("instruction_counter b: %d\n", instruction_counter);
                        instruction_counter = new_counter;
                        //printf("instruction_counter a: %d\n", instruction_counter);
                        seg_zero = Seq_get(s->segments, 0);
                }
                else {
                        instruction_counter++;
                }
                //printf("instruction_counter: %d\n", instruction_counter);
                // printf("seg_size: %d\n", seg_size);
                //printf("curr_instruction2: %x\n", seg_zero[instruction_counter ]);
                //program_counter = (uint32_t *) UArray_at(seg_zero, 
                //                                     instruction_counter);
                //program_counter = seg_zero[instruction_counter];
        }
}

/* Purpose: Unpacks the opcode from the passed-in instruction
 * Arguments: The instruction
 * Returns: The opcode
 */
uint32_t unpack_opcode(uint32_t instruction) {
        uint32_t opcode = instruction >> 28;//Bitpack_getu(instruction, 4, 28);
        //printf("opcode: %d\n", opcode);
        return opcode;
}

/* Purpose: Performs the load value instruction
 * Arguments: The special instruction and the registers
 * Returns: void
 */
void load_value(uint32_t curr_instruction) {
        int num_regi = curr_instruction << 4 >> 29;//Bitpack_getu(curr_instruction, 3, 25);
        //*(uint32_t *)UArray_at(registers, num_regi) = 
        registers[num_regi] = curr_instruction << 7 >> 7;//Bitpack_getu(curr_instruction, 25, 0);
}

/* Purpose: Calls certain functions depending on the opcode that is passed in
 * Arguments: Opcode, registers, Segments struct pointer, and the current
 * instruction
 * Returns: Returns -1 if load program was not called and the value in register
 * c otherwise so that run_instructions can properly assign the program counter
 */
int call_instruction(uint32_t opcode, Segments *s, 
                     uint32_t curr_instruction) {
        int c = -1;
        if (opcode == 13) {
                load_value(curr_instruction);
        }

        uint32_t ra = curr_instruction << 23 >> 29;//Bitpack_getu(curr_instruction, 3, 6);
        uint32_t rb = curr_instruction << 26 >> 29;//Bitpack_getu(curr_instruction, 3, 3);
        uint32_t rc = curr_instruction << 29 >> 29;//Bitpack_getu(curr_instruction, 3, 0);

        if (opcode == 0 || (2 < opcode && opcode < 7)) {
                perform_op(ra, rb, rc, opcode);
        }
        if ((0 < opcode && opcode < 3) || (7 < opcode && opcode < 10)) {
                perform_seg(ra, rb, rc, opcode, s);
        }
        if (opcode == 7) {
                free_all_segments(s);
                //UArray_free(&(registers));
                free(s);
                exit(0);
        }
        if (opcode == 10 || opcode == 11) {
                uint32_t value_c = registers[rc];//*(uint32_t *) UArray_at(registers, rc);
                perform_io(value_c, opcode);
        }
        if (opcode == 12) {
                uint32_t value_b = registers[rb];//*(uint32_t *) UArray_at(registers, rb);
                uint32_t value_c = registers[rc];//*(uint32_t *) UArray_at(registers, rc);
                load_program(s, value_b, value_c);
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
        uint32_t value_a = registers[a];//*(uint32_t *) UArray_at(registers, a);
        uint32_t value_b = registers[b];//*(uint32_t *) UArray_at(registers, b);
        uint32_t value_c = registers[c];//*(uint32_t *) UArray_at(registers, c);
        if (opcode == 0) {
               //*(uint32_t *) UArray_at(registers, a) = 
               registers[a] = conditional_move(value_a, value_b, value_c);
        }
        else if (opcode == 3) {
                uint32_t sum = add(value_b, value_c);
                registers[a] = sum;
                //*(uint32_t *) UArray_at(registers, a) = sum;
        }
        else if (opcode == 4) {
                uint32_t result = multiply(value_b, value_c);
                registers[a] = result;
                //*(uint32_t *) UArray_at(registers, a) = result;
        }
        else if (opcode == 5) {
                uint32_t result = divide(value_b, value_c);
                registers[a] = result;
                //*(uint32_t *) UArray_at(registers, a) = result;
        }
        else {
                uint32_t result = nand(value_b, value_c);
                registers[a] = result;
                //*(uint32_t *) UArray_at(registers, a) = result;
        }
}

/* Purpose: Calls the necessary segments function based on the opcode
 * Arguments: Registers and the number register for a, b, and c
 * Returns: void
 */
void perform_seg(uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode, Segments *s) {
        uint32_t value_a = registers[a];//*(uint32_t *) UArray_at(registers, a);
        uint32_t value_b = registers[b];//*(uint32_t *) UArray_at(registers, b);
        uint32_t value_c = registers[c];//*(uint32_t *) UArray_at(registers, c);
        if (opcode == 1) {
                //*(uint32_t *) UArray_at(registers, a) = 
                //printf("b, c, a: %d, %d, %d\n", value_b, value_c, a);
                registers[a] = segment_load(s, value_b , value_c+1);  
        }
        else if (opcode == 2) {
                uint32_t segmentID = value_a;
                //printf("offset: %d\n", value_c);
                segment_store(s, segmentID, value_b+1, value_c);
        }
        else if (opcode == 8) {
                //*(uint32_t *) UArray_at(registers, b) = 
                registers[b] = map_segment(s, value_c);
        }
        else {
                unmap_segment(s, value_c);
        }
}

/* Purpose: Calls the necessary io function based on the opcode
 * Arguments: Value to output and opcode
 * Returns: void
 */
void perform_io(uint32_t value, uint32_t opcode) {
        uint32_t output_val = value;
        if (opcode == 10) {
                output(output_val);
        } else {
                input();
        }
}

/* Purpose: Performs the load program instruction
 * Arguments: Segments struct pointer and the values in register b and c
 * Returns: void
 */
void load_program(Segments *s, uint32_t b, uint32_t c) {
        (void) c;
        if (b != 0) {
                //UArray_T sz = (UArray_T)(Seq_get(s->segments, 0));
                uint32_t *seg_zero = (uint32_t *)Seq_get(s->segments, 0);
                //printf("instruct 1: %x\n", seg_zero[0]);
                free(seg_zero);
                //UArray_free(&sz);
                //UArray_T temp = (UArray_T)Seq_get(s->segments, b);
                uint32_t *temp = (uint32_t *)Seq_get(s->segments, b);
                //UArray_T copy_seg = UArray_new(UArray_length(temp), 
                //                               sizeof(uint32_t));
                uint32_t *copy_seg = malloc(sizeof(uint32_t) * (temp[0]+1));
                int temp_size = temp[0];
                // for (unsigned j = 1; j < (temp[0]+1); j++){
                //         printf("instruction: %x\n", temp[j]);
                // }
                //printf("temp_size: %d\n", temp_size);
                for (int i = 0; i < temp_size + 1; i++){
                        //*(uint32_t *)UArray_at(copy_seg, i) = 
                        //        *(uint32_t * )UArray_at(temp, i);
                        copy_seg[i] = temp[i];
                        //printf("temp i: %x\n", temp[i]);
                }
                //printf("seg_size: %d\n", seg_size);
                seg_size = temp[0];
                Seq_put(s->segments, 0, copy_seg);
        }
}
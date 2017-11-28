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


void store_instruction(UArray_T seg_zero, uint32_t instruction, int open_index);
uint32_t unpack_opcode(uint32_t instruction);
int call_instruction(uint32_t opcode, UArray_T registers, Segments *s, 
                      uint32_t curr_instruction);
void perform_op(UArray_T registers, uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode);
void perform_seg(UArray_T registers, uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode, Segments *s);
void perform_io(uint32_t value, uint32_t opcode);
void unpack_one_register(uint32_t curr_instruction, UArray_T registers);

/* Purpose: Reads the instructions and stores them into segment zero
 * Arguments: File pointer and the filename
 * Returns: Updated pointer to the Segments struct
 */
Segments *read_instructions(FILE *fp, char *filename) {
        int open_index = 0;
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
        UArray_T seg_zero = (UArray_T) Seq_get(s->segments, 0);
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
        return s;
}

/* Purpose: Places instructions into segment zero
 * Arguments: Segment zero, the instruction, and the next open index to place 
 * the instruction into
 * Returns: void
 */
void store_instruction(UArray_T seg_zero, uint32_t instruction, 
                       int open_index) {
        *(uint32_t *) UArray_at(seg_zero, open_index) = instruction;
}

/* Purpose: Creates the registers array, updates the program counter, and 
 * goes through segment zero to call the function "call instructions" on each.
 * Arguments: Segments pointer
 * Returns: void
 */
void run_instructions(Segments *s) {
        UArray_T registers = UArray_new(8, sizeof(uint32_t));
        int instruction_counter = 0;
        
        UArray_T seg_zero = Seq_get(s->segments, 0);

        uint32_t *program_counter = (uint32_t *) UArray_at(seg_zero, 
                                                     instruction_counter);

        while (instruction_counter < UArray_length(seg_zero)) {
                uint32_t curr_instruction = *program_counter;
                uint32_t opcode = unpack_opcode(curr_instruction);
                int new_counter = call_instruction(opcode, registers, s, 
                                                   curr_instruction);
                if (new_counter != -1) {
                        instruction_counter = new_counter;
                        seg_zero = Seq_get(s->segments, 0);
                }
                else {
                        instruction_counter++;
                }
                program_counter = (uint32_t *) UArray_at(seg_zero, 
                                                     instruction_counter);
        }
}

/* Purpose: Unpacks the opcode from the passed-in instruction
 * Arguments: The instruction
 * Returns: The opcode
 */
uint32_t unpack_opcode(uint32_t instruction) {
        uint32_t opcode = Bitpack_getu(instruction, 4, 28);
        //printf("opcode: %d\n", opcode);
        return opcode;
}

/* Purpose: Performs the load value instruction
 * Arguments: The special instruction and the registers
 * Returns: void
 */
void load_value(uint32_t curr_instruction, UArray_T registers) {
        int num_regi = Bitpack_getu(curr_instruction, 3, 25);
        *(uint32_t *)UArray_at(registers, num_regi) = 
                        Bitpack_getu(curr_instruction, 25, 0);
}

/* Purpose: Calls certain functions depending on the opcode that is passed in
 * Arguments: Opcode, registers, Segments struct pointer, and the current
 * instruction
 * Returns: Returns -1 if load program was not called and the value in register
 * c otherwise so that run_instructions can properly assign the program counter
 */
int call_instruction(uint32_t opcode, UArray_T registers, Segments *s, 
                     uint32_t curr_instruction) {
        int c = -1;
        if (opcode == 13) {
                load_value(curr_instruction, registers);
        }

        uint32_t ra = Bitpack_getu(curr_instruction, 3, 6);
        uint32_t rb = Bitpack_getu(curr_instruction, 3, 3);
        uint32_t rc = Bitpack_getu(curr_instruction, 3, 0);

        if (opcode == 0 || (2 < opcode && opcode < 7)) {
                perform_op(registers, ra, rb, rc, opcode);
        }
        if ((0 < opcode && opcode < 3) || (7 < opcode && opcode < 10)) {
                perform_seg(registers, ra, rb, rc, opcode, s);
        }
        if (opcode == 7) {
                free_all_segments(s);
                UArray_free(&(registers));
                free(s);
                exit(0);
        }
        if (opcode == 10 || opcode == 11) {
                uint32_t value_c = *(uint32_t *) UArray_at(registers, rc);
                perform_io(value_c, opcode);
        }
        if (opcode == 12) {
                uint32_t value_b = *(uint32_t *) UArray_at(registers, rb);
                uint32_t value_c = *(uint32_t *) UArray_at(registers, rc);
                load_program(s, value_b, value_c);
                c = value_c;
        }
        return c;
}

/* Purpose: Calls the necessary operations function based on the opcode
 * Arguments: Registers and the number register for a, b, and c
 * Returns: void
 */
void perform_op(UArray_T registers, uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode) {
        uint32_t value_a = *(uint32_t *) UArray_at(registers, a);
        uint32_t value_b = *(uint32_t *) UArray_at(registers, b);
        uint32_t value_c = *(uint32_t *) UArray_at(registers, c);
        if (opcode == 0) {
               *(uint32_t *) UArray_at(registers, a) = 
                                conditional_move(value_a, value_b, value_c);
        }
        else if (opcode == 3) {
                uint32_t sum = add(value_b, value_c);
                //printf("sum: %u, %u\n", value_b, value_c);
                *(uint32_t *) UArray_at(registers, a) = sum;
        }
        else if (opcode == 4) {
                uint32_t result = multiply(value_b, value_c);
                *(uint32_t *) UArray_at(registers, a) = result;
        }
        else if (opcode == 5) {
                uint32_t result = divide(value_b, value_c);
                *(uint32_t *) UArray_at(registers, a) = result;
        }
        else {
                uint32_t result = nand(value_b, value_c);
                *(uint32_t *) UArray_at(registers, a) = result;
        }
}

/* Purpose: Calls the necessary segments function based on the opcode
 * Arguments: Registers and the number register for a, b, and c
 * Returns: void
 */
void perform_seg(UArray_T registers, uint32_t a, uint32_t b, uint32_t c, 
                uint32_t opcode, Segments *s) {
        uint32_t value_a = *(uint32_t *) UArray_at(registers, a);
        uint32_t value_b = *(uint32_t *) UArray_at(registers, b);
        uint32_t value_c = *(uint32_t *) UArray_at(registers, c);
        if (opcode == 1) {
                *(uint32_t *) UArray_at(registers, a) = 
                                segment_load(s, value_b, value_c);  
        }
        else if (opcode == 2) {
                uint32_t segmentID = value_a;
                segment_store(s, segmentID, value_b, value_c);
        }
        else if (opcode == 8) {
                *(uint32_t *) UArray_at(registers, b) = 
                                map_segment(s, value_c);
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
                UArray_T sz = (UArray_T)(Seq_get(s->segments, 0));
                UArray_free(&sz);
                UArray_T temp = (UArray_T)Seq_get(s->segments, b);
                UArray_T copy_seg = UArray_new(UArray_length(temp), 
                                               sizeof(uint32_t));
                for (int i = 0; i < UArray_length(temp); i++){
                        *(uint32_t *)UArray_at(copy_seg, i) = 
                                *(uint32_t * )UArray_at(temp, i);
                }
                Seq_put(s->segments, 0, copy_seg);

        }
}
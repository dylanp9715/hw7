/* 
 * operations.c
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * Implementation of the operation functions
 */

#include "operations.h"

const long divisor = 4294967296;

/* Purpose: Performs the conditional move
 * Arguments: The values in registers a, b, and c
 * Returns: The value in register b if c is not 0 and the value in register a 
 * otherwise
 */
uint32_t conditional_move(uint32_t a, uint32_t b, uint32_t c) {
        if (c != 0) {
                return b;
        }
        return a;
}

/* Purpose: Performs the add operation
 * Arguments: The values in registers b and c
 * Returns: The sum
 */
uint32_t add(uint32_t b, uint32_t c) {
        return (b + c) % divisor;
}

/* Purpose: Performs the multiply operation
 * Arguments: The values in registers b and c
 * Returns: The value of (b * c) mod 2^32
 */
uint32_t multiply(uint32_t b, uint32_t c) {
        return (b * c) % divisor;
}

/* Purpose: Performs the divide operation
 * Arguments: The values in registers b and c
 * Returns: The value of (b / c) mod 2^32
 */
uint32_t divide(uint32_t b, uint32_t c) {
        return (b/c);
}

/* Purpose: Performs the nand operation
 * Arguments: The values in registers b and c
 * Returns: The inverse of b & c
 */
uint32_t nand(uint32_t b, uint32_t c) { 
        return ~(b & c);
}
/* 
 * um.c
 * by Mike Pine and Dylan Park
 * HW 6
 *
 * Runs the UM 
 */

#include <stdio.h>
#include <stdlib.h>
#include "instruction_handler.h"

int main(int argc, char *argv[])
{
        (void) argc;
        FILE *fp = fopen(argv[1], "r");
        Segments *s = malloc(sizeof(Segments));
        s->openID = NULL;
        s->segments = NULL;
        s = read_instructions(fp, argv[1]);
        run_instructions(s);
}

#
# This Makefile is more verbose than necessary.  In each assignment
# we will simplify the Makefile using more powerful syntax and implicit rules.
#
# New syntax rules added:  wildcards (%.o, %.c) and special variables:
# $@ - the target of the rule
# $< - the first dependency
# $^ - all dependencies
#
# Last updated: February 4, 2016


############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I. -I/comp/40/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
CFLAGS = -g -std=c99 -Wall -O2 -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64

# Libraries needed for linking
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
# Only brightness requires the binary for pnmrdr.
LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt -lbitpack

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
#
# This bugs Mark, who dislikes false dependencies, but
# he agrees with Noah that you'll probably spend hours 
# debugging if you forget to put .h files in your 
# dependency list.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: segmentstest operationstest iotest um writetests


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

segmentstest: segmentstest.o segments.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

operationstest: operationstest.o operations.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

iotest: iotest.o io.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um: um.o instruction_handler.o bitpack.o segments.o operations.o io.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: labtest.o labwritetest.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f um io segments operations *.o
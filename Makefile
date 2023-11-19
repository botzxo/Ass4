# Use this Makefile as a starting point for your A4.

# Keep the first rule with the target "all", and change the name of the
# dependencies to whatever you like. These represent the four main programs
# that a complete assignment should produce. If you haven't completed them
# all, remove any that don't exist.

# Then, change the remaining rules to build each of those main programs,
# based on the names of your source and header files. For the targets that
# link the programs, make sure the target name matches the first object
# file name, without the ".o". And for the targets that compile the object
# files, make sure that the .o target and the first .c dependency match.

# Use of variables is optional but it will make things easier!

CC=clang
CFLAGS=-Wall # -g -DNDEBUG

# The default goal is to build all four programs

all: a4_test_mm_old a4_test_mm_new a4_test_kwic a4_kwic

# Targets to link all four programs

a4_test_mm_old: a4_test_mm_old.o a4_tests.o a4_multimap.o

a4_test_mm_new: a4_test_mm_new.o a4_tests.o a4_multimap.o

a4_test_kwic: a4_test_kwic.o a4_tests.o a4_kwic.o a4_multimap.o

a4_kwic: a4_kwic.o a4_main.o a4_multimap.o

# Targets to compile all object files

a4_test_mm_old.o: a4_test_mm_old.c a4_tests.h a4_multimap.h a4_boolean.h

a4_test_mm_new.o: a4_test_mm_new.c a4_tests.h a4_multimap.h a4_boolean.h

a4_test_kwic.o: a4_test_kwic.c a4_tests.h a4_kwic.h a4_multimap.h a4_boolean.h

a4_tests.o: a4_tests.c a4_tests.h a4_boolean.h

a4_multimap.o: a4_multimap.c a4_multimap.h a4_boolean.h

a4_kwic.o: a4_kwic.c a4_kwic.h a4_multimap.h a4_boolean.h

a4_main.o: a4_main.c a4_kwic.h a4_multimap.h a4_boolean.h

# Remove build products

clean:
	rm -f *.o a4_test_mm_old a4_test_mm_new a4_test_kwic a4_kwic

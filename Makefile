# Setup implicit rule to build object files
CC             = gcc
DEFAULT_CFLAGS = -Werror -Wall -Wextra -g
CFLAGS         = -std=c99 -pedantic $(DEFAULT_CFLAGS)


# Rules for tests
.PHONY: tests
tests:  tests/array_test tests/array_gnu_test tests/hash_test tests/list_test tests/tree_test
	./tests/array_test
	./tests/array_gnu_test
	./tests/hash_test
	./tests/list_test
	./tests/tree_test

array.o: array.c array.h
tests/array_test: tests/testing.o array.o

tests/array_gnu_test: CFLAGS = -std=gnu99 $(DEFAULT_CFLAGS)
tests/array_gnu_test: tests/testing.o array.o

hash.o: hash.c hash.h
tests/hash_test: tests/testing.o hash.o

list.o: list.c list.h
tests/list_test: tests/testing.o list.o

tree.o: tree.c tree.h
tests/tree_test: tests/testing.o tree.o


# Clean all files listed in .gitignore. Ensures this file
# is properly maintained.
clean:
	rm -fr `tr '\n' ' ' < .gitignore`
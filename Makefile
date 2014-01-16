# Setup implicit rule to build object files
CC             = gcc
DEFAULT_CFLAGS = -Werror -Wall -Wextra -g
CFLAGS         = -std=c99 -pedantic $(DEFAULT_CFLAGS)

# Rules for tests
.PHONY: tests
tests:  tests/array_test tests/array_gnu_test tests/hash_test
	./tests/array_test
	./tests/array_gnu_test
	./tests/hash_test

tests/array_test: tests/testing.o array.o

tests/array_gnu_test: CFLAGS = -std=gnu99 $(DEFAULT_CFLAGS)
tests/array_gnu_test: tests/testing.o array.o

tests/hash_test: tests/testing.o hash.o


# Clean all files listed in .gitignore. Ensures this file
# is properly maintained.
clean:
	rm -fr `tr '\n' ' ' < .gitignore`
include ../Makeparams

all: array_test hash_test
	./array_test
	./hash_test

testing.o: testing.h testing.c
	gcc $(GCC_FLAGS) -c testing.c

array_test: array_test.c testing.o
	cd ..; make array.o
	gcc $(GCC_FLAGS) array_test.c ../array.o testing.o -o array_test

hash_test: hash_test.c testing.o
	cd ..; make hash.o
	gcc $(GCC_FLAGS) hash_test.c ../hash.o testing.o -o hash_test
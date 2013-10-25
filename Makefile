include Makeparams

array.o: array.h array.c
	gcc $(GCC_FLAGS) -c array.c

hash.o: hash.h hash.c
	gcc $(GCC_FLAGS) -c hash.c

clean_all: clean
	cd tests; make clean
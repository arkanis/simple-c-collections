#include <stdio.h>
#include <stdint.h>
#include "testing.h"
#include "../array.h"

void print_float_array(array_p array){
	printf("array(%zu, [", array->length);
	for(size_t i = 0; i < array->length; i++)
		printf(" %f", array_data(array, float)[i]);
	printf(" ])\n");
}

void test_alloc(){
	array_p a = array_with(10, float);
	
	check_not_null(a);
	check_not_null(a->data);
	check_int(a->length, 10);
	check_int(a->element_size, sizeof(float));
	
	array_destroy(a);
}

void test_access(){
	array_p a = array_with(1, float);
	
	array_data(a, float)[0] = 3.141;
	float x = array_data(a, float)[0];
	check_float(x, 3.141, 0.001);
	
	array_destroy(a);
}

void test_append(){
	array_p a = array_with(0, float);
	check_int(a->length, 0);
	
	array_append(a, float, 3.141);
	check_int(a->length, 1);
	check_float(array_data(a, float)[0], 3.141, 0.001);
	
	array_append(a, float, 7.5);
	check_int(a->length, 2);
	check_float(array_data(a, float)[1], 7.5, 0.001);
	
	array_destroy(a);
}

void test_array_of(){
	array_p a = array_of(double);
	check_int(a->length, 0);
	check_int(a->element_size, sizeof(double));
	array_destroy(a);
}

void test_amortized_doubling(){
	array_p a = array_of(double);
	check_int(a->capacity, 0);
	
	array_append(a, double, 1.0);
	check_int(a->capacity, 1);
	array_append(a, double, 1.0);
	check_int(a->capacity, 2);
	array_append(a, double, 1.0);
	check_int(a->capacity, 4);
	array_append(a, double, 1.0);
	check_int(a->capacity, 4);
	
	array_resize(a, 800);
	check_int(a->capacity, 1024);
	
	array_resize(a, 15);
	check_int(a->capacity, 32);
	
	array_destroy(a);
}


bool compact_threshold_elem_is_empty(array_p array, size_t index){
	return array_data(array, int)[index] == 0;
}

void test_array_compact_threshold(){
	array_p a = array_with(10, int);
	array_data(a, int)[0] = 1;
	array_data(a, int)[1] = 1;
	array_data(a, int)[2] = 0;
	array_data(a, int)[3] = 1;
	array_data(a, int)[4] = 1;
	array_data(a, int)[5] = 0;
	array_data(a, int)[6] = 1;
	array_data(a, int)[7] = 1;
	array_data(a, int)[8] = 0;
	array_data(a, int)[9] = 1;
	check_int(a->length, 10);
	
	array_compact_threshold(a, 5, compact_threshold_elem_is_empty);
	check_int(a->length, 10);
	
	array_compact_threshold(a, 2, compact_threshold_elem_is_empty);
	check_int(a->length, 7);
	for(size_t i = 0; i < a->length; i++)
		check_int( array_data(a, int)[i], 1 );
	
	array_destroy(a);
}

bool array_find_comp1(array_p array, size_t index){ return array_elem(array, int, index) == 19; }
bool array_find_comp2(array_p array, size_t index){ return array_elem(array, int, index) == 26; }
bool array_find_comp3(array_p array, size_t index){ return array_elem(array, int, index) == 8;  }

void test_array_find(){
	array_p a = array_with(4, int);
	array_data(a, int)[0] = 7;
	array_data(a, int)[1] = 584;
	array_data(a, int)[2] = 19;
	array_data(a, int)[3] = 26;
	
	check_int(array_find(a, array_find_comp1), 2);
	check_int(array_find(a, array_find_comp2), 3);
	check_int(array_find(a, array_find_comp3), -1);
	
	array_destroy(a);
}

void test_array_remove(){
	array_p a = array_with(4, uint32_t);
	array_data(a, uint32_t)[0] = 0x11223344;
	array_data(a, uint32_t)[1] = 0x55667788;
	array_data(a, uint32_t)[2] = 0x99aabbcc;
	array_data(a, uint32_t)[3] = 0xddeeff00;
	check_int(a->length, 4);
	
	// Removing something in the middle
	array_remove(a, 1);
	check_int(a->length, 3);
	check_int(array_elem(a, uint32_t, 0), 0x11223344);
	check_int(array_elem(a, uint32_t, 1), 0x99aabbcc);
	check_int(array_elem(a, uint32_t, 2), 0xddeeff00);
	
	// Removing something at the end
	array_remove(a, 2);
	check_int(a->length, 2);
	check_int(array_elem(a, uint32_t, 0), 0x11223344);
	check_int(array_elem(a, uint32_t, 1), 0x99aabbcc);
	
	// Removing something at the beginning
	array_remove(a, 0);
	check_int(a->length, 1);
	check_int(array_elem(a, uint32_t, 1), 0x99aabbcc);
	
	array_destroy(a);
}

bool array_remove_func_is_null(array_p array, size_t index){ return array_elem(array, uint32_t, index) == 0; }

void test_array_remove_func(){
	array_p a = array_with(5, uint32_t);
	array_data(a, uint32_t)[0] = 0;
	array_data(a, uint32_t)[1] = 0x11223344;
	array_data(a, uint32_t)[2] = 0;
	array_data(a, uint32_t)[3] = 0x99aabbcc;
	array_data(a, uint32_t)[4] = 0;
	check_int(a->length, 5);
	
	array_remove_func(a, array_remove_func_is_null);
	check_int(a->length, 2);
	check_int(array_elem(a, uint32_t, 0), 0x11223344);
	check_int(array_elem(a, uint32_t, 1), 0x99aabbcc);
	
	array_destroy(a);
}

typedef struct {
	int a, b, c;
} stuff_t, *stuff_p;

void test_array_append_ptr(){
	array_p a = array_of(stuff_t);
	
	stuff_p stuff = array_append_ptr(a);
	stuff->a = 1;
	stuff->b = 2;
	stuff->c = 3;
	
	check_int(a->length, 1);
	
	stuff = array_append_ptr(a);
	*stuff = (stuff_t){ 4, 5, 6 };
	
	check_int(a->length, 2);
	
	check_int( array_elem(a, stuff_t, 0).a, 1);
	check_int( array_elem(a, stuff_t, 0).b, 2);
	check_int( array_elem(a, stuff_t, 0).c, 3);
	check_int( array_elem(a, stuff_t, 1).a, 4);
	check_int( array_elem(a, stuff_t, 1).b, 5);
	check_int( array_elem(a, stuff_t, 1).c, 6);
	
	array_destroy(a);
}

void test_array_elem_ptr(){
	array_p a = array_of(int);
	array_append(a, int, 8);
	array_append(a, int, 17);
	check_int(a->length, 2);
	
	int* p = NULL;
	p = array_elem_ptr(a, 0);
	check( *p == 8 );
	p = array_elem_ptr(a, 1);
	check( *p == 17 );
	
	array_destroy(a);
}

// Only test the features requireing GNU extention when we're compiling in GNU C mode
#ifndef __STRICT_ANSI__

void test_array_find_val(){
	array_p a = array_with(4, int);
	array_data(a, int)[0] = 7;
	array_data(a, int)[1] = 584;
	array_data(a, int)[2] = 19;
	array_data(a, int)[3] = 26;
	
	check_int(array_find_val(a, int, 19), 2);
	check_int(array_find_val(a, int, 26), 3);
	check_int(array_find_val(a, int, 8), -1);
	
	array_destroy(a);
}

void test_array_find_expr(){
	array_p a = array_with(4, int);
	array_data(a, int)[0] = 7;
	array_data(a, int)[1] = 584;
	array_data(a, int)[2] = 19;
	array_data(a, int)[3] = 26;
	
	check_int(array_find_expr(a, int, (x * 2) == 38), 2);
	check_int(array_find_expr(a, int, (x / 2) == 13), 3);
	check_int(array_find_expr(a, int, x == 8), -1);
	
	array_destroy(a);
}

void test_array_remove_val(){
	array_p a = array_with(4, int);
	array_data(a, int)[0] = 7;
	array_data(a, int)[1] = 584;
	array_data(a, int)[2] = 19;
	array_data(a, int)[3] = 26;
	
	array_remove_val(a, int, 19);
	check_int(a->length, 3);
	check_int(array_elem(a, uint32_t, 0), 7);
	check_int(array_elem(a, uint32_t, 1), 584);
	check_int(array_elem(a, uint32_t, 2), 26);

	array_remove_val(a, int, 7);
	array_remove_val(a, int, 26);
	check_int(a->length, 1);
	check_int(array_elem(a, uint32_t, 0), 584);
	
	array_destroy(a);
}

void test_array_remove_expr(){
	array_p a = array_with(4, int);
	array_data(a, int)[0] = 7;
	array_data(a, int)[1] = 584;
	array_data(a, int)[2] = 19;
	array_data(a, int)[3] = 26;
	
	array_remove_expr(a, int, x % 2 == 0);
	check_int(a->length, 2);
	check_int(array_elem(a, uint32_t, 0), 7);
	check_int(array_elem(a, uint32_t, 1), 19);
	
	array_remove_expr(a, int, x < 10);
	check_int(a->length, 1);
	check_int(array_elem(a, uint32_t, 0), 19);
	
	array_destroy(a);
}



#endif

int main(){
	run(test_alloc);
	run(test_access);
	run(test_append);
	run(test_array_of);
	run(test_amortized_doubling);
	run(test_array_compact_threshold);
	run(test_array_find);
	run(test_array_remove);
	run(test_array_remove_func);
	run(test_array_append_ptr);
	run(test_array_elem_ptr);
	
#	ifndef __STRICT_ANSI__
	run(test_array_find_val);
	run(test_array_find_expr);
	run(test_array_remove_val);
	run(test_array_remove_expr);
#	endif
	
	return show_report();
}
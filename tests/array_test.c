#include <stdio.h>
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

int main(){
	run(test_alloc);
	run(test_access);
	run(test_append);
	run(test_array_of);
	run(test_amortized_doubling);
	run(test_array_compact_threshold);
	return show_report();
}
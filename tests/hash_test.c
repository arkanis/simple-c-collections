#include <stdio.h>
#include "testing.h"
#include "../hash.h"

// Internal function of hash.c declared here to make it accessable to testing
size_t snap_to_prime(size_t x);

void test_alloc(){
	hash_p h = hash_new(10, sizeof(float));
	
	check_not_null(h);
	check_not_null(h->slots);
	check_int(h->capacity, 10);
	check_int(h->length, 0);
	check_int(h->value_size, sizeof(float));
	
	hash_destroy(h);
	
	h = hash_of(double);
	check_not_null(h);
	check_int(h->value_size, sizeof(double));
	hash_destroy(h);
	
	h = hash_with(10, uint8_t);
	check_not_null(h);
	check_int(h->value_size, sizeof(uint8_t));
	hash_destroy(h);
}

void test_put_and_get_new_elem(){
	hash_p h = hash_new(10, sizeof(float));
	check_int(h->length, 0);
	
	hash_put(h, 42, float, 3.141);
	check_int(h->length, 1);
	check_float(hash_get(h, 42, float), 3.141, 0.001);
	
	hash_destroy(h);
}

void test_put_with_overwrite(){
	hash_p h = hash_new(10, sizeof(float));
	
	hash_put(h, 17, float, 2.718);
	check_int(h->length, 1);
	check_float(hash_get(h, 17, float), 2.718, 0.001);
	
	hash_put(h, 17, float, 3.141);
	check_int(h->length, 1);
	check_float(hash_get(h, 17, float), 3.141, 0.001);
	
	hash_destroy(h);
}

void test_get_non_existing_element(){
	hash_p h = hash_new(10, sizeof(float));
	
	check_int(h->length, 0);
	check( hash_get_ptr(h, 17) == NULL );
	check_int(h->length, 0);
	
	hash_destroy(h);
}

void test_remove(){
	hash_p h = hash_new(10, sizeof(float));
	check_int(h->length, 0);
	
	hash_put(h,   1, float, 11);
	hash_put(h, 564, float, 12);
	hash_put(h, 976, float, 13);
	check_int(h->length, 3);
	check_float( hash_get(h, 564, float), 12, 0.1 );
	
	hash_remove(h, 564);
	check_int(h->length, 2);
	check( hash_get_ptr(h, 564) == NULL );
	
	hash_destroy(h);
}

void check_hash_contents(hash_p hash, int64_t keys[], int values[], size_t length){
	size_t found_element_count = 0;
	bool checklist[length];
	for(size_t i = 0; i < length; i++)
		checklist[i] = false;
	
	for(hash_elem_t elem = hash_start(hash); elem != NULL; elem = hash_next(hash, elem)){
		int64_t k = hash_key(elem);
		int v = hash_value(elem, int);
		
		size_t i;
		for(i = 0; i < length; i++){
			if (keys[i] == k && checklist[i] == false){
				check_int(v, values[i]);
				checklist[i] = true;
				break;
			}
		}
		
		if (i == length)
			check_msg(false, "failed to find a value for key %ld", k);
		
		found_element_count++;
	}
	
	check_int(found_element_count, length);
}

void test_iteration(){
	int64_t keys[] = { 1, 564, 976};
	int values[] =   {11,  12,  13};
	size_t element_count = sizeof(values) / sizeof(values[0]);
	
	// Put test elements into hash
	hash_p h = hash_new(10, sizeof(int));
	for(size_t i = 0; i < element_count; i++)
		hash_put(h, keys[i], int, values[i]);
	check_int(h->length, element_count);
	
	check_hash_contents(h, keys, values, element_count);
	
	hash_destroy(h);
}

void test_contains(){
	hash_p h = hash_with(5, float);
	hash_put(h, 17, float, 3.14);
	hash_put(h, 6,  float, 7.1);
	check_int(h->length, 2);
	
	check( hash_contains(h, 17) == true );
	check( hash_contains(h, 18) == false );
	check( hash_contains(h, 6)  == true );
	check( hash_contains(h, 0)  == false );
	
	hash_destroy(h);
}

void test_remove_element_in_iteration(){
	hash_p h = hash_with(5, int);
	hash_put(h, 1, int, 11);
	hash_put(h, 564, int, 12);
	hash_put(h, 976, int, 13);
	check_int(h->length, 3);
	
	for(hash_elem_t elem = hash_start(h); elem != NULL; elem = hash_next(h, elem)){
		if ( hash_key(elem) == 564 )
			hash_remove_elem(h, elem);
	}
	
	check_int(h->length, 2);
	check( hash_contains(h, 1) );
	check( !hash_contains(h, 564) );
	check( hash_contains(h, 976) );
	
	hash_destroy(h);
}

void test_resize(){
	hash_p h = hash_new(0, sizeof(float));
	check_int(h->capacity, 0);
	check_int(h->length, 0);
	
	// Resize it from 0 length
	hash_resize(h, 10);
	check_int(h->capacity, 10);
	check_int(h->length, 0);
	check_not_null(h->slots);
	
	// Add test elements to the hash
	int64_t keys[] = { 1, 564, 976};
	int values[] =   {11,  12,  13};
	size_t element_count = sizeof(values) / sizeof(values[0]);
	
	for(size_t i = 0; i < element_count; i++)
		hash_put(h, keys[i], int, values[i]);
	check_int(h->length, element_count);
	check_int(h->capacity, 10);
	check_hash_contents(h, keys, values, element_count);
	
	// Grow it again
	hash_resize(h, 20);
	check_int(h->capacity, 20);
	check_int(h->length, 3);
	check_not_null(h->slots);
	check_hash_contents(h, keys, values, element_count);
	
	// Shrink it
	hash_resize(h, 5);
	check_int(h->capacity, 5);
	check_int(h->length, 3);
	check_not_null(h->slots);
	check_hash_contents(h, keys, values, element_count);
	
	hash_destroy(h);
}

void test_automatic_prime_resize(){
	hash_p h = hash_with(5, float);
	check_int(h->capacity, 5);
	
	// Let hash grow
	hash_put(h, 3, int, 2);
	hash_put(h, 5, int, 5);
	hash_put(h, 8, int, 21);
	check_int(h->length, 3);
	check_int(h->capacity, 5);
	hash_put(h, 12, int, 144);
	check_int(h->length, 4);
	check_int(h->capacity, 11);
	hash_put(h, 13, int, 233);
	hash_put(h, 15, int, 610);
	hash_put(h, 16, int, 987);
	hash_put(h, 19, int, 4181);
	check_int(h->length, 8);
	check_int(h->capacity, 11);
	hash_put(h, 20, int, 6765);
	check_int(h->length, 9);
	check_int(h->capacity, 23);
	
	// Shrink it again
	hash_remove(h, 3);
	hash_remove(h, 5);
	hash_remove(h, 8);
	hash_remove(h, 12);
	check_int(h->length, 5);
	check_int(h->capacity, 23);
	hash_remove(h, 13);
	check_int(h->length, 4);
	check_int(h->capacity, 11);
	hash_remove(h, 15);
	check_int(h->length, 3);
	check_int(h->capacity, 11);
	hash_remove(h, 16);
	check_int(h->length, 2);
	check_int(h->capacity, 5);
	
	hash_destroy(h);
}

void test_snap_to_prime(){
	size_t samples[] = {
		3, 5,
		6, 11,
		38, 47,
		72, 97,
		830, 1597,
		16777230, 16777259
	};
	
	for(size_t i = 0; i < sizeof(samples) / sizeof(size_t); i += 2)
		check_int(snap_to_prime(samples[i]), samples[i+1]);
}

void test_dict(){
	dict_p d = dict_of(int);
	
	dict_put(d, "foo", int, 0);
	dict_put(d, "bar", int, 8);
	dict_put(d, "hurdelgrumpf", int, 15);
	check_int(d->length, 3);
	
	check_int( dict_get(d, "foo", int), 0 );
	check_int( dict_get(d, "hurdelgrumpf", int), 15 );
	check_int( dict_get(d, "bar", int), 8 );
	
	dict_destroy(d);
}

/**
 * Bug test: While resizing a dict was reset to a hash (int key instead of string key). This
 * caused the string key to be a NULL pointer.
 */
void test_dict_resize(){
	dict_p d = dict_with(1, int);
	
	dict_put(d, "foo", int, 1572);
	check_int(d->length, 1);
	
	// Have to use iteration to get invalid string key pointer. With dict_get we would map to
	// the same invalid hash and everything would work more or less.
	for(dict_elem_t e = dict_start(d); e; e = dict_next(d, e)) {
		const char* key = dict_key(e);
		const int value = dict_value(e, int);
		
		check_not_null(key);
		check_str(key, "foo");
		check_int(value, 1572);
	}
	
	dict_destroy(d);
}

int main(){
	run(test_alloc);
	run(test_put_and_get_new_elem);
	run(test_put_with_overwrite);
	run(test_get_non_existing_element);
	run(test_remove);
	run(test_iteration);
	run(test_contains);
	run(test_remove_element_in_iteration);
	run(test_resize);
	run(test_automatic_prime_resize);
	run(test_snap_to_prime);
	run(test_dict);
	run(test_dict_resize);
	return show_report();
}
#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct {
	size_t length, capacity;
	size_t element_size;
	void* data;
} array_t, *array_p;


#define array_with(length, type)  array_new(length, sizeof(type))
#define array_of(type)            array_new(0, sizeof(type))

#define array_data(array, type)           ((type*)array->data)
#define array_append(array, type, value)  (*((type*)array_resize(array, array->length + 1)) = (value))

array_p array_new(size_t length, size_t element_size);
void    array_destroy(array_p array);
void*   array_resize(array_p array, size_t new_length);

// Removes empty elements from an array if more than `empty_elements_threshold` are empty.
typedef bool (*array_is_elem_empty_t)(array_p array, size_t index);
void    array_compact_threshold(array_p array, size_t empty_elements_threshold, array_is_elem_empty_t func);


/*

Further ideas:

// Searches for the first occurence of value in the array
size_t array_find(array, value)

// searches in region of the array. length = 0 means all remaining elements after start.
// a negative start value means an index from the end (-1 is the last element)
// array_find_in(a, 42, 0, 0)  => searches entire array
// array_find_in(a, 42, 5, 10) => search from index 5 the next 10 elements
// array_find_in(a, 42, -1, 0) => search the entire array backwards
// array_find_in(a, 42, -1, 5) => search in the last 5 elements
size_t array_find_in(array, value, start, length)

// same meaning of the start and length parameter as above
array_p array_slice(array, start, length);
array_p array_copy_slice(array, start, length);


void array_remove(array, index);
void array_remove_slice(array, start, length);

*/
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>


typedef struct {
	size_t length, capacity;
	size_t element_size;
	void* data;
} array_t, *array_p;


#define array_with(length, type)  array_new(length, sizeof(type))
#define array_of(type)            array_new(0, sizeof(type))

#define array_data(array, type)           ((type*)array->data)
#define array_elem(array, type, index)    (array_data(array, type)[index])
#define array_append(array, type, value)  (*((type*)array_resize(array, array->length + 1)) = (value))

array_p array_new(size_t length, size_t element_size);
void    array_destroy(array_p array);
void*   array_resize(array_p array, size_t new_length);

typedef bool (*array_elem_func_t)(array_p array, size_t index);

// Removes empty elements from an array if more than `empty_elements_threshold` are empty.
void    array_compact_threshold(array_p array, size_t empty_elements_threshold, array_elem_func_t is_empty_function);

// Searches for the first occurence of value in the array. Returns -1 if no matching element was found.
ssize_t array_find(array_p array, array_elem_func_t check_function);

// Removes one element from the array
void    array_remove(array_p array, size_t index);

void    array_remove_func(array_p array, array_elem_func_t func);

// This stuff requires the statement as expression GCC extention. Therefore only compile
// it when not in strict C mode.
#ifndef __STRICT_ANSI__

#define array_find_val(array, type, value)  ({        \
	ssize_t index = -1;                               \
	for(size_t i = 0; i < array->length; i++) {       \
		if ( array_elem(array, type, i) == value ) {  \
			index = i;                                \
			break;                                    \
		}                                             \
	}                                                 \
	index;                                            \
})

// In the expression the current element is available as the variable "x"
#define array_find_expr(array, type, expression)  ({  \
	ssize_t index = -1;                               \
	for(size_t i = 0; i < array->length; i++) {       \
		type x = array_elem(array, type, i);          \
		if (expression) {                             \
			index = i;                                \
			break;                                    \
		}                                             \
	}                                                 \
	index;                                            \
})

#define array_remove_val(array, type, value)  ({     \
	bool elem_func(array_p a, size_t index){         \
		return array_elem(a, type, index) == value;  \
	}                                                \
	array_remove_func(array, elem_func);             \
})

// In the expression the current element is available as the variable "x"
#define array_remove_expr(array, type, expression)  ({    \
	bool elem_func(array_p a, size_t index){              \
		type x = array_elem(a, type, index);              \
		return (expression);                              \
	}                                                     \
	array_remove_func(array, elem_func);                  \
})


#endif

/*

Further ideas:


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


void array_remove_slice(array, start, length);

Strange stuff / Bugs:

- array.h causes bug in sys/types.h header file. GCC error:
  
  In file included from array.h:5:0,
                 from register_allocator.h:5,
                 from register_allocator.c:2:
  /usr/include/x86_64-linux-gnu/sys/types.h:34:18: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘u_char’
  
  In that case include array.h before the rest. Not yet looked into it.

*/
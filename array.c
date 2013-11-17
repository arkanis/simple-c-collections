#include <stdlib.h>
#include <string.h>  // for memcpy
#include "array.h"

array_p array_new(size_t length, size_t element_size){
	array_p array = malloc(sizeof(array_t));
	
	if (array == NULL)
		return NULL;
	
	array->length = length;
	array->capacity = length;
	array->element_size = element_size;
	array->data = malloc(length * element_size);
	
	if (array->data == NULL){
		free(array);
		return NULL;
	}
	
	return array;
}

void array_destroy(array_p array){
	free(array->data);
	free(array);
}

/**
 * Resize should return the pointer to the newly appended memory block on success.
 * NULL is returned when the array shrinks or the realloc failed.
 */
void* array_resize(array_p array, size_t new_length){
	size_t new_capacity = array->capacity;
	
	if (new_capacity == 0) {
		new_capacity = new_length;
	} else {
		// If the requested length is greater than the capacity double it until we get
		// a capacity large enough.
		while(new_length > new_capacity)
			new_capacity *= 2;
		// If the length is smaller than a quater of the capacity half it until we get
		// a capacity small enough.
		while(new_length < new_capacity / 4)
			new_capacity /= 2;
	}
	
	// Realloc the array data if the capacity has to change
	if (new_capacity != array->capacity) {
		void* reallocated_data = realloc(array->data, new_capacity * array->element_size);
		if (reallocated_data == NULL)
			return NULL;
		
		array->data = reallocated_data;
		array->capacity = new_capacity;
	}
	
	size_t old_length = array->length;
	array->length = new_length;
	
	return (old_length < new_length) ? (char*)array->data + (old_length * array->element_size) : NULL;
}

void array_compact_threshold(array_p array, size_t empty_elements_threshold, array_is_elem_empty_t is_elem_empty_func){
	// First check if enough elements are empty
	size_t empty_elements = 0;
	for(size_t index = 0; index < array->length; index++) {
		if ( is_elem_empty_func(array, index) ) {
			empty_elements++;
			
			if (empty_elements >= empty_elements_threshold)
				break;
		}
	}
	
	if (empty_elements < empty_elements_threshold)
		return;
	
	// Then overwrite empty elements with the following filled ones
	size_t compacted_index = 0;
	for(size_t index = 0; index < array->length; index++) {
		if ( !is_elem_empty_func(array, index) ) {
			if (index != compacted_index)
				memcpy((char*)array->data + compacted_index * array->element_size, (char*)array->data + index * array->element_size, array->element_size);
			
			compacted_index++;
		}
	}
	
	// And finally chop off the empty end of the array
	array_resize(array, compacted_index);
}
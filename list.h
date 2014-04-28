#pragma once

/**

# A simple double linked list

A list has a `first` and `last` pointer to the first and last node. Each node has a
`prev` and `next` pointer to its siblings.


// Creating and destroying lists

list_p list = list_of(int);
list_destroy(list);


// Adding nodes to the list and accessing the first and last nodes

list_prepend(list, int, 8);
list_append(list, int, 7);

list_first(list, int);    // -> 8
list_last(list, int);     // -> 7

list_remove_first(list);  // removes 8
list_remove_last(list);   // removes 7


// For functions that add or read nodes there are also void pointer version.
// They return a pointer to the value of the node.

list_prepend_ptr(list);  // -> void pointer to the newly prepended memory block sizeof(int) bytes large
list_append_ptr(list);   // -> void pointer to the newly appended memory block sizeof(int) bytes large

list_first_ptr(list);    // -> void pointer to the value of the first node
list_last_ptr(list);     // -> void pointer to the value of the last node


// Normal iteration (removing nodes during iteration will segfault!)

for (list_node_p n = list->first; n != NULL; n = n->next) {
	int value = list_value(n, int);
	...
}


// Removing nodes during iteration
// Can't dereference `node` after the `list_remove()` call. Therefore get the next
// pointer right at the start of each iteration.

for (list_node_p node = list->first, next = NULL; node != NULL; node = next) {
	next = node->next;
	
	if ( list_value(node, int) % 2 == 0 )
		list_remove(list, node);
}


// Per node operations, e.g. using a node during iteration

list_node_p node = list->first->next;
list_value(node, int);                   // -> 7

list_insert_before(list, node, int, 6);  // adds 6 before 7
list_insert_after(list, node, int, 8);   // adds 8 after 7

list_remove(list, node);                 // removes 7
list_new_node(list);                     // New unwired node (type list_node_p, next and prev both NULL) with undefined
                                         // value. You have to wire it up yourself.

// Again there are void* versions for functions that add or read nodes.
// They return the memory address of the block that stores the value.

list_value_ptr(node);                // -> the memory address where 7 is stored
list_insert_before_ptr(list, node);  // adds a node before 7 and returns the address where you can store the new value
list_insert_after_ptr(list, node);   // adds a node after 7 and returns the address where you can store the new value

*/

#include <stddef.h>
#include <stdbool.h>


typedef struct list_node_s list_node_t, *list_node_p;
struct list_node_s {
	struct list_node_s* prev;
	struct list_node_s* next;
};

typedef struct {
	size_t element_size;
	list_node_p first, last;
} list_t, *list_p;


#define              list_of(               type)                              list_new(sizeof(type))
list_p               list_new(              size_t element_size);
void                 list_destroy(          list_p list);

#define              list_prepend(          list, type, value)                 (*((type*)list_prepend_ptr(list)) = (value))
#define              list_append(           list, type, value)                 (*((type*)list_append_ptr(list)) = (value))
void*                list_prepend_ptr(      list_p list);
void*                list_append_ptr(       list_p list);

size_t               list_count(            list_p list);
void                 list_clear(            list_p list);
void                 list_remove_first(     list_p list);
void                 list_remove_last(      list_p list);

#define              list_first(            list, type)                         list_value(list->first, type)
#define              list_last(             list, type)                         list_value(list->last,  type)

#define              list_value(            node, type)                         ( *((type*)list_value_ptr(node)) )
static inline  void* list_value_ptr(        list_node_p node)                   { return node + 1; }

list_node_p          list_new_node(         list_p list);
void                 list_remove(           list_p list, list_node_p node);
#define              list_insert_before(    list, node, type, value)            (*((type*)list_insert_before_ptr(list, node)) = (value))
#define              list_insert_after(     list, node, type, value)            (*((type*)list_insert_after_ptr(list, node)) = (value))
void*                list_insert_before_ptr(list_p list, list_node_p node);
void*                list_insert_after_ptr( list_p list, list_node_p node);
#include <stdlib.h>
#include <string.h>

#include "list.h"


list_p list_new(size_t element_size) {
	list_p list = malloc(sizeof(list_t));
	
	list->element_size = element_size;
	list->first = NULL;
	list->last = NULL;
	
	return list;
}

void list_destroy(list_p list) {
	list_clear(list);
	free(list);
}

void* list_prepend_ptr(list_p list) {
	list_node_p node = list_new_node(list);
	
	node->prev = NULL;
	node->next = list->first;
	
	if (list->first)
		list->first->prev = node;
	
	list->first = node;
	if (list->last == NULL)
		list->last = node;
	
	return node + 1;
}

void* list_append_ptr(list_p list) {
	list_node_p node = list_new_node(list);
	
	node->prev = list->last;
	node->next = NULL;
	
	if (list->last)
		list->last->next = node;
	
	list->last = node;
	if (list->first == NULL)
		list->first = node;
	
	return node + 1;
}

void list_clear(list_p list) {
	list_node_p next = NULL;
	for(list_node_p n = list->first; n != NULL; n = next) {
		next = n->next;
		free(n);
	}
	
	list->first = NULL;
	list->last = NULL;
}

void list_remove_first(list_p list) {
	list_node_p node = list->first;
	
	if (node->next)
		node->next->prev = NULL;
	
	list->first = node->next;
	if (list->last == node)
		list->last = NULL;
	
	free(node);
}

void list_remove_last(list_p list) {
	list_node_p node = list->last;
	
	if (node->prev)
		node->prev->next = NULL;
	
	list->last = node->prev;
	if (list->first == node)
		list->first = NULL;
	
	free(node);
}

void list_remove(list_p list, list_node_p node) {
	if (node == list->first) {
		list_remove_first(list);
	} else if (node == list->last) {
		list_remove_last(list);
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		free(node);
	}
}

void* list_insert_before_ptr(list_p list, list_node_p target_node) {
	if (target_node == NULL)
		return NULL;
	else if (target_node == list->first)
		return list_prepend_ptr(list);
	
	list_node_p new_node = list_new_node(list);
	
	new_node->prev = target_node->prev;
	new_node->next = target_node;
	
	if (target_node->prev)
		target_node->prev->next = new_node;
	target_node->prev = new_node;
	
	return new_node + 1;
}

void* list_insert_after_ptr(list_p list, list_node_p target_node) {
	if (target_node == NULL)
		return NULL;
	else if (target_node == list->last)
		return list_append_ptr(list);
	
	list_node_p new_node = list_new_node(list);
	
	new_node->prev = target_node;
	new_node->next = target_node->next;
	
	if (target_node->next)
		target_node->next->prev = new_node;
	target_node->next = new_node;
	
	return new_node + 1;
}

list_node_p list_new_node(list_p list) {
	list_node_p new_node = malloc(sizeof(list_node_t) + list->element_size);
	
	new_node->prev = NULL;
	new_node->next = NULL;
	
	return new_node;
}

size_t list_count(list_p list) {
	size_t count = 0;
	
	for(list_node_p n = list->first; n != NULL; n = n->next)
		count++;
	
	return count;
}
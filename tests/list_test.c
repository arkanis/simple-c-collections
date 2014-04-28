#include <stdio.h>
#include <stdint.h>
#include "testing.h"
#include "../list.h"


void test_prepend_and_clear() {
	list_p list = list_of(int);
	
	check_null(list->first);
	check_null(list->last);
	check_int(list->element_size, 4);
	
	list_prepend(list, int, 15);
	check_not_null(list->first);
	check_not_null(list->last);
	check(list->first == list->last);
	check_int( list_value(list->first, int), 15 );
	
	list_prepend(list, int, 8);
	check(list->first != list->last);
	check_int( list_value(list->first, int), 8 );
	check_int( list_value(list->last, int), 15 );
	
	list_prepend(list, int, 0);
	check(list->first != list->last);
	
	list_node_p node = list->first;
	check( node->prev == NULL );
	check( node->next == list->last->prev );
	check_int( list_value(node, int),        0 );
	
	node = node->next;
	check( node->prev == list->first );
	check( node->next == list->last  );
	check_int( list_value(node, int),   8 );
	
	node = node->next;
	check( node->prev == list->first->next );
	check( node->next == NULL );
	check_int( list_value(node, int), 15 );
	
	list_clear(list);
	check_null(list->first);
	check_null(list->last);
	
	list_destroy(list);
}

void test_append() {
	list_p list = list_of(int);
	
	check_null(list->first);
	check_null(list->last);
	check_int(list->element_size, 4);
	
	list_append(list, int, 15);
	check_not_null(list->first);
	check_not_null(list->last);
	check(list->first == list->last);
	check_int( list_value(list->first, int), 15 );
	
	list_append(list, int, 8);
	check(list->first != list->last);
	check_int( list_value(list->first, int), 15 );
	check_int( list_value(list->last, int),   8 );
	
	list_append(list, int, 0);
	check(list->first != list->last);
	
	list_node_p node = list->first;
	check( node->prev == NULL );
	check( node->next == list->last->prev );
	check_int( list_value(node, int),        15 );
	
	node = node->next;
	check( node->prev == list->first );
	check( node->next == list->last  );
	check_int( list_value(node, int),   8 );
	
	node = node->next;
	check( node->prev == list->first->next );
	check( node->next == NULL );
	check_int( list_value(node, int), 0 );
	
	list_destroy(list);
}

void test_list_remove_first() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	check_int( list_value(list->first, int), 0  );
	check_int( list_value(list->last,  int), 15 );
	list_remove_first(list);
	
	check_int( list_value(list->first, int), 8  );
	check_int( list_value(list->last,  int), 15 );
	list_remove_first(list);
	
	check_int( list_value(list->first, int), 15 );
	check_int( list_value(list->last,  int), 15 );
	list_remove_first(list);
	
	check_null(list->first);
	check_null(list->last);
	
	list_destroy(list);
}

void test_list_remove_last() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	check_int( list_value(list->first, int), 0  );
	check_int( list_value(list->last,  int), 15 );
	list_remove_last(list);
	
	check_int( list_value(list->first, int), 0  );
	check_int( list_value(list->last,  int), 8  );
	list_remove_last(list);
	
	check_int( list_value(list->first, int), 0  );
	check_int( list_value(list->last,  int), 0  );
	list_remove_last(list);
	
	check_null(list->first);
	check_null(list->last);
	
	list_destroy(list);
}

void test_list_first_and_last() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	check_int( list_first(list, int), 0 );
	check_int( list_last(list, int), 15 );
	
	list_destroy(list);
}

void test_iteration() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	size_t i = 0;
	for (list_node_p n = list->first; n != NULL; n = n->next) {
		switch(i) {
			case 0: check_int( list_value(n, int),  0 ); break;
			case 1: check_int( list_value(n, int),  8 ); break;
			case 2: check_int( list_value(n, int), 15 ); break;
			default: check_msg(false, "there is no 4th element!");
		}
		i++;
	}
	
	i = 0;
	for (list_node_p n = list->last; n != NULL; n = n->prev) {
		switch(i) {
			case 0: check_int( list_value(n, int), 15 ); break;
			case 1: check_int( list_value(n, int),  8 ); break;
			case 2: check_int( list_value(n, int),  0 ); break;
			default: check_msg(false, "there is no 4th element!");
		}
		i++;
	}
	
	list_destroy(list);
}

void test_list_remove() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	list_remove(list, list->first->next);
	check_int( list_first(list, int), 0 );
	check_int( list_last(list, int), 15 );
	check( list->first->next == list->last );
	check( list->last->prev == list->first );
	
	list_remove(list, list->first);
	check_int( list_first(list, int), 15 );
	check( list->first == list->last );
	
	list_remove(list, list->last);
	check_null(list->first);
	check_null(list->last);
	
	list_destroy(list);
}

void test_list_remove_during_iteration() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 1);
	list_append(list, int, 2);
	list_append(list, int, 3);
	list_append(list, int, 4);
	list_append(list, int, 5);
	list_append(list, int, 6);
	
	for (list_node_p node = list->first, next = NULL; node != NULL; node = next) {
		next = node->next;
		if ( list_value(node, int) % 2 == 0 )
			list_remove(list, node);
	}
	
	check_int( list_value(list->first,       int), 1 );
	check_int( list_value(list->first->next, int), 3 );
	check_int( list_value(list->last,        int), 5 );
	
	for (list_node_p node = list->first, next = NULL; node != NULL; node = next) {
		next = node->next;
		list_remove(list, node);
	}
	
	check_null(list->first);
	check_null(list->last);
	
	list_destroy(list);
}

void test_list_insert_before() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	
	list_insert_before(list, list->first, int, 15);
	check( list->first != list->last );
	check( list->first->prev == NULL );
	check( list->first->next == list->last );
	check( list->last->prev  == list->first );
	check( list->last->next  == NULL );
	check_int( list_first(list, int), 15 );
	check_int( list_last(list,  int),  0 );
	
	list_insert_before(list, list->last, int, 8);
	list_node_p node = list->first->next;
	check( node->prev == list->first );
	check( node->next == list->last  );
	check( list->first->next == node );
	check( list->last->prev  == node );
	check_int( list_first(list, int), 15 );
	check_int( list_last(list,  int),  0 );
	
	list_destroy(list);
}

void test_list_insert_after() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	
	list_insert_after(list, list->first, int, 15);
	check( list->first != list->last );
	check( list->first->prev == NULL );
	check( list->first->next == list->last );
	check( list->last->prev  == list->first );
	check( list->last->next  == NULL );
	check_int( list_first(list, int),  0 );
	check_int( list_last(list,  int), 15 );
	
	list_insert_after(list, list->first, int, 8);
	list_node_p node = list->first->next;
	check( node->prev == list->first );
	check( node->next == list->last  );
	check( list->first->next == node );
	check( list->last->prev  == node );
	check_int( list_first(list, int),  0 );
	check_int( list_last(list,  int), 15 );
	
	list_destroy(list);
}

void test_list_count() {
	list_p list = list_of(int);
	
	list_append(list, int, 0);
	list_append(list, int, 8);
	list_append(list, int, 15);
	
	check_int( list_count(list), 3 );
	
	list_append(list, int, 7);
	list_append(list, int, 21);
	
	check_int( list_count(list), 5 );
	
	list_remove_first(list);
	list_remove_last(list);
	
	check_int( list_count(list), 3 );
	
	list_destroy(list);
}


int main(){
	run(test_prepend_and_clear);
	run(test_append);
	run(test_list_remove_first);
	run(test_list_remove_last);
	run(test_list_first_and_last);
	run(test_iteration);
	run(test_list_remove);
	run(test_list_remove_during_iteration);
	run(test_list_insert_before);
	run(test_list_insert_after);
	run(test_list_count);
	
	return show_report();
}
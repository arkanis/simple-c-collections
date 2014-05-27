#include "testing.h"
#include "../tree.h"


void test_flat_prepend_and_clear() {
	tree_p tree = tree_new();
	
	check_null(tree->parent);
	check_null(tree->first_child);
	check_null(tree->last_child);
	check_null(tree->prev);
	check_null(tree->next);
	
	tree_p n1 = tree_prepend(tree, int, 7);
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->parent == tree);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	check(n1->first_child == NULL);
	check(n1->last_child  == NULL);
	check_int( tree_value(n1, int), 7 );
	
	tree_p n2 = tree_prepend(tree, int, 15);
	check(tree->first_child == n2);
	check(tree->last_child  == n1);
	check(n2->prev == NULL);
	check(n2->next == n1);
	check(n1->prev == n2);
	check(n1->next == NULL);
	check_int( tree_value(n2, int), 15 );
	
	tree_p n3 = tree_prepend(tree, int, 8);
	check(tree->first_child == n3);
	check(tree->last_child  == n1);
	check(n3->prev == NULL);
	check(n3->next == n2);
	check(n2->prev == n3);
	check(n2->next == n1);
	check(n1->prev == n2);
	check(n1->next == NULL);
	check_int( tree_value(n3, int), 8 );
	
	tree_clear(tree);
	check_null(tree->first_child);
	check_null(tree->last_child);
	
	tree_destroy(tree);
}

void test_nested_prepend() {
	tree_p tree = tree_new();
	
	tree_p n1 = tree_prepend(tree, int, 0);
	check_null(n1->first_child);
	check_null(n1->last_child);
	check_int( tree_value(n1, int), 0 );
	
	tree_p n11 = tree_prepend(n1, int, 8);
	check(n1->first_child == n11);
	check(n1->last_child  == n11);
	check_null(n11->first_child);
	check_null(n11->last_child);
	check_int( tree_value(n11, int), 8 );
	
	tree_p n111 = tree_prepend(n11, int, 15);
	check(n11->first_child == n111);
	check(n11->last_child  == n111);
	check_null(n111->first_child);
	check_null(n111->last_child);
	check_int( tree_value(n111, int), 15 );
	
	tree_destroy(tree);
}

void test_append() {
	tree_p tree = tree_new();
	
	tree_p n1 = tree_append(tree, int, 0);
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->parent == tree);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	check(n1->first_child == NULL);
	check(n1->last_child  == NULL);
	check_int( tree_value(n1, int), 0 );
	
	tree_p n2 = tree_append(tree, int, 8);
	check(tree->first_child == n1);
	check(tree->last_child  == n2);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == NULL);
	check_int( tree_value(n2, int), 8 );
	
	tree_p n3 = tree_append(tree, int, 15);
	check(tree->first_child == n1);
	check(tree->last_child  == n3);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == NULL);
	check_int( tree_value(n3, int), 15 );
	
	tree_p n21 = tree_append(n2, int, 21);
	check(n2->first_child == n21);
	check(n2->last_child  == n21);
	check(n21->prev == NULL);
	check(n21->next == NULL);
	
	tree_clear(tree);
	check_null(tree->first_child);
	check_null(tree->last_child);
	
	tree_destroy(tree);
}

void test_remove_first() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 0);
	tree_p n2 = tree_append(tree, int, 8);
	tree_p n3 = tree_append(tree, int, 15);
	check(tree->first_child == n1);
	check(tree->last_child  == n3);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == NULL);
	
	tree_remove_first(tree);
	check(tree->first_child == n2);
	check(tree->last_child  == n3);
	check(n2->prev == NULL);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == NULL);
	
	tree_remove_first(tree);
	check(tree->first_child == n3);
	check(tree->last_child  == n3);
	check(n3->prev == NULL);
	check(n3->next == NULL);
	
	tree_remove_first(tree);
	check(tree->first_child == NULL);
	check(tree->last_child  == NULL);
	
	tree_destroy(tree);
}

void test_remove_last() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 0);
	tree_p n2 = tree_append(tree, int, 8);
	tree_p n3 = tree_append(tree, int, 15);
	check(tree->first_child == n1);
	check(tree->last_child  == n3);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == NULL);
	
	tree_remove_last(tree);
	check(tree->first_child == n1);
	check(tree->last_child  == n2);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == NULL);
	
	tree_remove_last(tree);
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	
	tree_remove_last(tree);
	check(tree->first_child == NULL);
	check(tree->last_child  == NULL);
	
	tree_destroy(tree);
}

void test_remove() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 0);
	tree_p n2 = tree_append(tree, int, 8);
	tree_p n3 = tree_append(tree, int, 15);
	
	check(tree->first_child == n1);
	check(tree->last_child  == n3);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == NULL);
	
	tree_remove(n2);
	check(tree->first_child == n1);
	check(tree->last_child  == n3);
	check(n1->prev == NULL);
	check(n1->next == n3);
	check(n3->prev == n1);
	check(n3->next == NULL);
	
	tree_remove(n3);
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	
	tree_remove(n1);
	check(tree->first_child == NULL);
	check(tree->last_child  == NULL);
	
	tree_destroy(tree);
}

void test_insert_before() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 15);
	
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	
	tree_p n2 = tree_insert_before(n1, int, 0);
	check(tree->first_child == n2);
	check(tree->last_child  == n1);
	check(n2->prev == NULL);
	check(n2->next == n1);
	check(n1->prev == n2);
	check(n1->next == NULL);
	
	tree_p n3 = tree_insert_before(n1, int, 8);
	check(tree->first_child == n2);
	check(tree->last_child  == n1);
	check(n2->prev == NULL);
	check(n2->next == n3);
	check(n3->prev == n2);
	check(n3->next == n1);
	check(n1->prev == n3);
	check(n1->next == NULL);
	
	tree_destroy(tree);
}

void test_insert_after() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 15);
	
	check(tree->first_child == n1);
	check(tree->last_child  == n1);
	check(n1->prev == NULL);
	check(n1->next == NULL);
	
	tree_p n2 = tree_insert_after(n1, int, 0);
	check(tree->first_child == n1);
	check(tree->last_child  == n2);
	check(n1->prev == NULL);
	check(n1->next == n2);
	check(n2->prev == n1);
	check(n2->next == NULL);
	
	tree_p n3 = tree_insert_after(n1, int, 0);
	check(tree->first_child == n1);
	check(tree->last_child  == n2);
	check(n1->prev == NULL);
	check(n1->next == n3);
	check(n3->prev == n1);
	check(n3->next == n2);
	check(n2->prev == n3);
	check(n2->next == NULL);
	
	tree_destroy(tree);
}

typedef struct {
	int type;
	union {
		char v4[4];
		char v6[16];
	} addr;
} test_struct_t, *test_struct_p;

void test_structs_as_value() {
	tree_p tree = tree_new();
	
	            tree_prepend(tree, test_struct_t, ((test_struct_t){ .type = 4, .addr.v4 = "abcd" }));
	tree_p n2 = tree_append (tree, test_struct_t, ((test_struct_t){ .type = 6, .addr.v6 = "abc_123_xyz_uvw_" }));
	            tree_insert_after(n2, test_struct_t, ((test_struct_t){ .type = 4, .addr.v4 = "1270" }));
	            tree_insert_before(n2, test_struct_t, ((test_struct_t){ .type = 4, .addr.v4 = "1926" }));
	
	// list: abcd, 1926, abc_123_xyz_uvw_, 1270
	tree_p node = tree->first_child;
	check( tree_value(node, test_struct_t).addr.v4[3] == 'd' );
	node = node->next;
	check( tree_value(node, test_struct_t).addr.v4[2] == '2' );
	node = node->next;
	check( tree_value(node, test_struct_t).addr.v6[14] == 'w' );
	node = node->next;
	check( tree_value(node, test_struct_t).addr.v4[2] == '7' );
	node = node->next;
	check_null(node);
	
	tree_destroy(tree);
}

void test_simple_preorder_iteration() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_p n13 = tree_append(n1, int, 13);
			tree_p n131 = tree_append(n13, int, 131);
			tree_p n132 = tree_append(n13, int, 132);
	tree_p n2 = tree_append(tree, int, 2);
	
	// Iterate a subtree. Checks that we don't break out of it.
	tree_p n = tree_first_pre(n1);	check(n == n13);
	n = tree_next_pre(n1, n);		check(n == n131);
	n = tree_next_pre(n1, n);		check(n == n132);
	n = tree_next_pre(n1, n);		check(n == NULL);
	// Check NULL handling
	n = tree_next_pre(n1, n);		check(n == NULL);
	
	// Try to iterate over the entire tree
	n = tree_first_pre(tree);		check(n == n1);
	n = tree_next_pre(tree, n);		check(n == n13);
	n = tree_next_pre(tree, n);		check(n == n131);
	n = tree_next_pre(tree, n);		check(n == n132);
	n = tree_next_pre(tree, n);		check(n == n2);
	n = tree_next_pre(tree, n);		check(n == NULL);
	
	// Just a final check in the meant to be used way
	int counter = 0;
	for(tree_p n = tree_first_pre(n1); n != NULL; n = tree_next_pre(n1, n)) {
		counter++;
	}
	check_int(counter, 3);
	
	tree_destroy(tree);
}

void test_preorder_skip_children() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_p n13 = tree_append(n1, int, 13);
			              tree_append(n13, int, 131);
			tree_p n132 = tree_append(n13, int, 132);
		tree_p n14 = tree_append(n1, int, 14);
	tree_append(tree, int, 2);
	
	tree_p n = tree_first_pre(n1);		check(n == n13);
	n = tree_skip_children_pre(n1, n);	check(n == n132);
	n = tree_next_pre(n1, n);			check(n == n14);
	n = tree_skip_children_pre(n1, n);	check(n == n14);
	n = tree_next_pre(n1, n);			check(n == NULL);
	
	tree_destroy(tree);
}

void test_simple_postorder_iteration() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_p n13 = tree_append(n1, int, 13);
			tree_p n131 = tree_append(n13, int, 131);
			tree_p n132 = tree_append(n13, int, 132);
		tree_p n14 = tree_append(n1, int, 14);
	tree_append(tree, int, 2);
	
	// Iterate a subtree. Checks that we don't break out of it.
	tree_p n = tree_first_post(n1);	check(n == n131);
	n = tree_next_post(n1, n);		check(n == n132);
	n = tree_next_post(n1, n);		check(n == n13);
	n = tree_next_post(n1, n);		check(n == n14);
	n = tree_next_post(n1, n);		check(n == NULL);
	// Check NULL handling
	n = tree_next_post(n1, n);		check(n == NULL);
	
	// Just a final check in the meant to be used way
	int counter = 0;
	for(tree_p n = tree_first_post(n1); n != NULL; n = tree_next_post(n1, n)) {
		counter++;
	}
	check_int(counter, 4);
	
	tree_destroy(tree);
}

void test_complex_iteration() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_p n11 = tree_append(n1, int, 11);
		tree_p n12 = tree_append(n1, int, 12);
		tree_p n13 = tree_append(n1, int, 13);
			tree_p n131 = tree_append(n13, int, 131);
			tree_p n132 = tree_append(n13, int, 132);
		tree_p n14 = tree_append(n1, int, 14);
	tree_append(tree, int, 2);
	
	tree_iterator_t it = tree_first(n1);
	check(it.root  == n1);
	check(it.node  == n11);
	check(it.level == 0);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n12);
	check(it.level == 0);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n13);
	check(it.level == 0);
	check(it.flags == TREE_PRE_VISIT);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n131);
	check(it.level == 1);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n132);
	check(it.level == 1);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n13);
	check(it.level == 0);
	check(it.flags == TREE_POST_VISIT);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n14);
	check(it.level == 0);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == NULL);
	check(it.level == 0);
	check(it.flags == 0);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == NULL);
	check(it.level == 0);
	check(it.flags == 0);
	
	// Final "in the wild" usage test
	int count = 0;
	for(tree_iterator_t it = tree_first(n1); it.node != NULL; it = tree_next(it)) {
		count++;
	}
	check_int(count, 7);
	
	tree_destroy(tree);
}

void test_skip_children() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_p n11 = tree_append(n1, int, 11);
		tree_p n12 = tree_append(n1, int, 12);
		tree_p n13 = tree_append(n1, int, 13);
			              tree_append(n13, int, 131);
			tree_p n132 = tree_append(n13, int, 132);
	tree_append(tree, int, 2);
	
	tree_iterator_t it = tree_first(n1);
	check(it.root  == n1);
	check(it.node  == n11);
	check(it.level == 0);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n12);
	check(it.level == 0);
	check(it.flags == TREE_LEAF);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == n13);
	check(it.level == 0);
	check(it.flags == TREE_PRE_VISIT);
	
	it = tree_skip_children(it);
	check(it.root  == n1);
	check(it.node  == n132);
	check(it.level == 1);
	check(it.flags == TREE_LEAF);
	
	it = tree_skip_children(it);
	check(it.root  == n1);
	check(it.node  == n13);
	check(it.level == 0);
	check(it.flags == TREE_POST_VISIT);
	
	it = tree_next(it);
	check(it.root  == n1);
	check(it.node  == NULL);
	check(it.level == 0);
	check(it.flags == 0);
	
	tree_destroy(tree);
}

void test_examples() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		tree_append(n1, int, 11);
		tree_append(n1, int, 12);
	tree_p n2 = tree_append(tree, int, 2);
		tree_append(n2, int, 21);
	
	printf("\n");
	for(tree_p n = tree_first_pre(tree); n != NULL; n = tree_next_pre(tree, n))
		printf("%d ", tree_value(n, int));
	printf("\n");
	
	for(tree_p n = tree_first_post(tree); n != NULL; n = tree_next_post(tree, n))
		printf("%d ", tree_value(n, int));
	printf("\n");
	
	for(tree_iterator_t it = tree_first(tree); it.node != NULL; it = tree_next(it)) {
		if (it.flags & TREE_PRE_VISIT)
			printf("%d: [ ", tree_value(it.node, int));
		else if (it.flags & TREE_POST_VISIT)
			printf("] ");
		else
			printf("%d ", tree_value(it.node, int));
	}
	printf("\n");
}

void test_tree_building() {
	tree_p tree = tree_new();
	tree_p n1 = tree_append(tree, int, 1);
		             tree_append(n1, int, 11);
		             tree_append(n1, int, 12);
		tree_p n13 = tree_append(n1, int, 13);
			              tree_append(n13, int, 131);
			              tree_append(n13, int, 132);
		             tree_append(n1, int, 14);
	tree_p n2 = tree_append(tree, int, 2);
		             tree_append(n2, int, 21);
		             tree_append(n2, int, 22);
	tree_p n3 = tree_append(tree, int, 3);
		             tree_append(n3, int, 31);
	
	
	printf("\n");
	for(tree_iterator_t it = tree_first(tree); it.node != NULL; it = tree_next(it)) {
		if (it.flags & TREE_POST_VISIT)
			continue;
		
		bool ends[it.level];
		tree_p in = it.node->parent;
		for(int i = it.level - 1; i >= 0; i--) {
			ends[i] = (in->next == NULL);
			in = in->parent;
		}
		
		for(size_t i = 0; i < it.level; i++)
			printf("%s ", ends[i] ? " " : "│");
		printf("%s %d\n", (it.node->next) ? "├" : "└", tree_value(it.node, int));
	}
	
	tree_destroy(tree);
}

int main(){
	run(test_flat_prepend_and_clear);
	run(test_nested_prepend);
	run(test_append);
	run(test_remove_first);
	run(test_remove_last);
	run(test_remove);
	run(test_insert_before);
	run(test_insert_after);
	run(test_structs_as_value);
	run(test_simple_preorder_iteration);
	run(test_preorder_skip_children);
	run(test_simple_postorder_iteration);
	run(test_complex_iteration);
	run(test_skip_children);
	
	// Test cases just contain example code and other nice stuff
	//run(test_examples);
	//run(test_tree_building);
	
	return show_report();
}
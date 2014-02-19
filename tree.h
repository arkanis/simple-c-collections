#pragma once

/**

// Creating and destroying a tree

tree_p tree = tree_of(int);
tree_destroy(tree);


// Adding nodes to the tree and accessing the first and last nodes

tree_prepend(tree, int, 8);
tree_append(tree, int, 7);

tree_remove_first(tree);  // removes 8
tree_remove_last(tree);   // removes 7
tree_clear(tree);         // removes all child nodes of tree or a node

// For functions that add or read nodes there are also void pointer version.
// They return a pointer to the value of the node.

tree_prepend_ptr(tree, int);  // -> void pointer to the newly prepended memory block sizeof(int) bytes large
tree_append_ptr(tree, int);   // -> void pointer to the newly appended memory block sizeof(int) bytes large

// There are also versions where you can specify the size in bytes. In case
// you want to allocate nodes of different sizes.

tree_prepend_block(tree, 20);  // -> void pointer to 20 byte value block of newly prepended node
tree_append_block(tree, 20);   // -> void pointer to 20 byte value block of newly appended node


// Per node operations, e.g. using a node during iteration or navigating
// through the tree. Each node is a valid tree itself. So if you want to
// work on a subtree just pick the subtrees root node as tree.

tree_p node = tree->first_child->next;
tree_value(tree, int);             // -> 7

tree_insert_before(node, int, 6);  // adds 6 before 7
tree_insert_after(node, int, 8);   // adds 8 after 7

tree_remove(node);                 // removes 7
tree_node_of(int);                 // New unwired node (type tree_p, all pointers set to NULL) with undefined
                                   // value. You have to wire it up yourself.
tree_node_of_size(20);             // Same as tree_node_of() but with the size of the value instead of the
                                   // value type.

// Again there are void* versions for functions that add, read or create
// nodes. They return the memory address of the block that stores the value.

list_value_ptr(node);               // -> the memory address where 7 is stored
tree_insert_before_ptr(node, int);  // adds a node before 7 and returns the address where you can store the new value
tree_insert_after_ptr(node, int);   // adds a node after 7 and returns the address where you can store the new value

// And block versions for functions that create nodes

tree_insert_before_block(node, 20);
tree_insert_after_ptr(node, 20);


// A tree can be iteratated in three ways: preorder (top-down), postorder (bottom-up)
// or with an iterator that does both and gives you additional information (e.g. current
// tree level).

tree_p tree = tree_new();
tree_p n1 = tree_append(tree, int, 1);
	tree_append(n1, int, 11);
	tree_append(n1, int, 12);
tree_p n2 = tree_append(tree, int, 2);
	tree_append(n2, int, 21);

// Preorder traversal (top-down, parents before children)

for(tree_p n = tree_first_pre(tree); n != NULL; n = tree_next_pre(tree, n))
	printf("%d ", tree_value(n, int));

// Results in "1 11 12 2 21 "

// Postorder traversal (bottom-up, children before parents)

for(tree_p n = tree_first_post(tree); n != NULL; n = tree_next_post(tree, n))
	printf("%d ", tree_value(n, int));

// Results in "11 12 1 21 2 "

// Preorder and postorder traversal with an iterator. This iterator contains
// the current node and additional information.

for(tree_iterator_t it = tree_first(tree); it.node != NULL; it = tree_next(it)) {
	if (it.flags & TREE_PRE_VISIT)
		printf("%d: [ ", tree_value(it.node, int));
	else if (it.flags & TREE_POST_VISIT)
		printf("] ");
	else
		printf("%d ", tree_value(it.node, int));
}

// Results in "1: [ 11 12 ] 2: [ 21 ] "

// The preorder and iterator versions also allow to skip the children of a specifc
// node (tree_skip_children_pre() and tree_skip_children()). These functions
// advance to the last child so the for loop automatically continues with the
// node after that.


# Implementation notes

The last assignment "(tree)->first_child = (tree)->first_child" is only there
to suppress an unused-value error (comes with -Wall). An "(tree)->first_child"
would suffice to return the new node.
Same applies to tree_append(), tree_insert_before() and tree_insert_after() macros.

*/

#include <stddef.h>
#include <stdint.h>


typedef struct tree_s tree_t, *tree_p;
struct tree_s {
	tree_p parent;
	tree_p first_child, last_child;
	tree_p prev, next;
};


tree_p               tree_new(              );
void                 tree_destroy(          tree_p tree);

// Last assignment in macros suppresses unused-value error. See implementation notes above.
#define              tree_prepend(          tree, type, value)                 (*((type*)tree_prepend_block((tree), sizeof(type))) = (value), (tree)->first_child = (tree)->first_child)
#define              tree_prepend_ptr(      tree, type)                        tree_prepend_block((tree), sizeof(type))
void*                tree_prepend_block(    tree_p tree, size_t value_size);
#define              tree_append(           tree, type, value)                 (*((type*)tree_append_block((tree), sizeof(type))) = (value), (tree)->last_child = (tree)->last_child)
#define              tree_append_ptr(       tree, type)                        tree_append_block((tree), sizeof(type))
void*                tree_append_block(     tree_p tree, size_t value_size);

void                 tree_clear(            tree_p tree);
void                 tree_remove_first(     tree_p tree);
void                 tree_remove_last(      tree_p tree);



#define tree_node_of(type)                     tree_node_of_size(sizeof(type))
tree_p  tree_node_of_size(size_t value_size);

#define              tree_value(            node, type)                         ( *((type*)tree_value_ptr(node)) )
static inline  void* tree_value_ptr(        tree_p node)                   { return node + 1; }

void                 tree_remove(           tree_p node);

// Last assignment in macros suppresses unused-value error. See implementation notes above.
#define              tree_insert_before(           node, type, value)                 (*((type*)tree_insert_before_block((node), sizeof(type))) = (value), (node)->prev = (node)->prev)
#define              tree_insert_before_ptr(       node, type)                        tree_insert_before_block((node), sizeof(type))
void*                tree_insert_before_block(tree_p node, size_t value_size);
#define              tree_insert_after(           node, type, value)                 (*((type*)tree_insert_after_block((node), sizeof(type))) = (value), (node)->next = (node)->next)
#define              tree_insert_after_ptr(       node, type)                        tree_insert_after_block((node), sizeof(type))
void*                tree_insert_after_block( tree_p node, size_t value_size);


tree_p tree_first_pre(tree_p tree);
tree_p tree_next_pre(tree_p tree, tree_p node);
tree_p tree_skip_children_pre(tree_p tree, tree_p node);
tree_p tree_first_post(tree_p tree);
tree_p tree_next_post(tree_p tree, tree_p node);

typedef struct {
	tree_p node, root;
	size_t level;
	uint16_t flags;
} tree_iterator_t, *tree_iterator_p;

#define TREE_PRE_VISIT  (1 << 0)
#define TREE_POST_VISIT (1 << 1)
#define TREE_LEAF       (1 << 2)

tree_iterator_t tree_first(tree_p tree);
tree_iterator_t tree_next(tree_iterator_t iterator);
tree_iterator_t tree_skip_children(tree_iterator_t iterator);
#include <stdlib.h>
#include <string.h>

#include "tree.h"


tree_p tree_new() {
	return tree_node_of_size(0);
}

void tree_destroy(tree_p tree) {
	tree_clear(tree);
	free(tree);
}


void* tree_prepend_block(tree_p tree, size_t value_size) {
	tree_p node = tree_node_of_size(value_size);
	
	node->parent = tree;
	node->prev = NULL;
	node->next = tree->first_child;
	
	if (tree->first_child)
		tree->first_child->prev = node;
	
	tree->first_child = node;
	if (tree->last_child == NULL)
		tree->last_child = node;
	
	return node + 1;
}

void* tree_append_block(tree_p tree, size_t value_size) {
	tree_p node = tree_node_of_size(value_size);
	
	node->parent = tree;
	node->prev = tree->last_child;
	node->next = NULL;
	
	if (tree->last_child)
		tree->last_child->next = node;
	
	tree->last_child = node;
	if (tree->first_child == NULL)
		tree->first_child = node;
	
	return node + 1;
}



void tree_clear(tree_p tree) {
	tree_p next = NULL;
	for(tree_p n = tree->first_child; n != NULL; n = next) {
		tree_clear(n);
		next = n->next;
		free(n);
	}
	
	tree->first_child = NULL;
	tree->last_child = NULL;
}

void tree_remove_first(tree_p tree) {
	tree_p node = tree->first_child;
	
	if (node->next)
		node->next->prev = NULL;
	
	tree->first_child = node->next;
	if (tree->last_child == node)
		tree->last_child = NULL;
	
	free(node);
}

void tree_remove_last(tree_p tree) {
	tree_p node = tree->last_child;
	
	if (node->prev)
		node->prev->next = NULL;
	
	tree->last_child = node->prev;
	if (tree->first_child == node)
		tree->first_child = NULL;
	
	free(node);
}



tree_p tree_node_of_size(size_t value_size) {
	tree_p node = malloc(sizeof(tree_t) + value_size);
	memset(node, 0, sizeof(tree_t) + value_size);
	return node;
}

void tree_remove(tree_p node) {
	tree_clear(node);
	
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
	
	if (node->parent) {
		if (node->parent->first_child == node)
			node->parent->first_child = node->next;
		if (node->parent->last_child == node)
			node->parent->last_child = node->prev;
	}
	
	free(node);
}

void* tree_insert_before_block(tree_p target_node, size_t value_size) {
	if (target_node == NULL)
		return NULL;
	
	tree_p new_node = tree_node_of_size(value_size);
	new_node->parent = target_node->parent;
	
	new_node->prev = target_node->prev;
	new_node->next = target_node;
	
	if (target_node->prev)
		target_node->prev->next = new_node;
	else if (target_node->parent)
		target_node->parent->first_child = new_node;
	target_node->prev = new_node;
	
	return new_node + 1;
}

void* tree_insert_after_block(tree_p target_node, size_t value_size) {
	if (target_node == NULL)
		return NULL;
	
	tree_p new_node = tree_node_of_size(value_size);
	new_node->parent = target_node->parent;
	
	new_node->prev = target_node;
	new_node->next = target_node->next;
	
	if (target_node->next)
		target_node->next->prev = new_node;
	else if (target_node->parent)
		target_node->parent->last_child = new_node;
	target_node->next = new_node;
	
	return new_node + 1;
}


tree_p tree_first_pre(tree_p tree) {
	if (!tree)
		return NULL;
	return tree->first_child;
}

tree_p tree_next_pre(tree_p tree, tree_p node) {
	if (!node)
		return NULL;
	
	if (node->first_child) {
		return node->first_child;
	} else if (node->next) {
		return node->next;
	} else {
		// Go up until we're at the end of our subtree or find a next node
		tree_p n = node->parent;
		while(n != NULL) {
			if (n == tree)
				return NULL;
			if (n->next)
				return n->next;
			n = n->parent;
		}
		return NULL;
	}
}

tree_p tree_skip_children_pre(tree_p tree, tree_p node) {
	tree = tree;
	if (!node->last_child)
		return node;
	return node->last_child;
}


tree_p tree_first_post(tree_p tree) {
	if (!tree)
		return NULL;
	
	tree_p node = tree->first_child;
	while(node->first_child)
		node = node->first_child;
	
	return node;
}

tree_p tree_next_post(tree_p tree, tree_p node) {
	if (!node)
		return NULL;
	
	if (node->next) {
		node = node->next;
		while(node->first_child)
			node = node->first_child;
		
		return node;
	} else if (node->parent == tree) {
		return NULL;
	}
	
	return node->parent;
}

tree_iterator_t tree_first(tree_p tree) {
	tree_iterator_t it = { 0 };
	
	if (!tree)
		return it;
	
	it.root  = tree;
	it.node  = tree->first_child;
	if (it.node)
		it.flags = (it.node->first_child) ? TREE_PRE_VISIT : TREE_LEAF;
	
	return it;
}

tree_iterator_t tree_next(tree_iterator_t it) {
	if (!it.node)
		return it;
	
	if (it.flags & TREE_PRE_VISIT) {
		it.node = it.node->first_child;
		it.level++;
		it.flags = (it.node->first_child) ? TREE_PRE_VISIT : TREE_LEAF;
	} else if (it.flags & TREE_LEAF || it.flags & TREE_POST_VISIT) {
		if (it.node->next) {
			it.node = it.node->next;
			it.flags = (it.node->first_child) ? TREE_PRE_VISIT : TREE_LEAF;
		} else if (it.node->parent != it.root) {
			it.node = it.node->parent;
			it.level--;
			it.flags = TREE_POST_VISIT;
		} else {
			it.node = NULL;
			it.flags = 0;
		}
	}
	
	return it;
}

tree_iterator_t tree_skip_children(tree_iterator_t it) {
	if (!it.node)
		return it;
	
	if (it.flags & TREE_PRE_VISIT) {
		it.node = it.node->last_child;
		it.level++;
		it.flags = (it.node->first_child) ? TREE_POST_VISIT : TREE_LEAF;
	} else {
		it = tree_next(it);
	}
	
	return it;
}
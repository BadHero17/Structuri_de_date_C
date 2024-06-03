#ifndef BST_HEAP_AVL_H
#define BST_HEAP_AVL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

typedef struct bst_node_t bst_node_t;
struct  bst_node_t {
	/* left child */
	bst_node_t *left;

	/* right child */
	bst_node_t *right;

	/* data contained by the node */
	void *data;
};

typedef struct bst_tree_t bst_tree_t;
struct bst_tree_t {
	/* root of the tree */
	bst_node_t  *root;

	 /* size of the data contained by the nodes */
	size_t data_size;

	/* function used for sorting the keys */
	int	(*cmp)(const void *key1, const void *key2);
};

typedef struct avl_node_t avl_node_t;
struct  avl_node_t {
	/* left child */
	avl_node_t *left;

	/* right child */
	avl_node_t *right;

	/* data contained by the node */
	void *data;

	unsigned char height;
};

typedef struct avl_tree_t avl_tree_t;
struct avl_tree_t {
	/* root of the tree */
	avl_node_t  *root;

	 /* size of the data contained by the nodes */
	size_t data_size;

	/* function used for sorting the keys */
	int	(*cmp)(const void *key1, const void *key2);

	int size;
};

unsigned char max(unsigned char a, unsigned char b);
bst_tree_t *bst_tree_create(size_t data_size,
	int (*cmp_f)(const void *, const void *));
void bst_tree_insert(bst_tree_t *bst_tree, void *data);
void bst_tree_remove(bst_tree_t *bst_tree, void *data);
void bst_tree_free(bst_tree_t *bst_tree, void (*free_data)(void *));
void bst_tree_print_inorder(bst_tree_t* bst_tree, void (*print_data)(void*));


avl_tree_t *avl_tree_create(size_t data_size,
	int (*cmp_f)(const void *, const void *));
void turn_right(void *ptr, char side);
void turn_left(void *ptr, char side);
void __avl_tree_insert(avl_node_t *avl_node, void *data, int data_size, int (*cmp)(const void *key1, const void *key2), int *succes);
void avl_tree_insert(avl_tree_t *avl_tree, void *data);
void __avl_tree_remove(avl_node_t *avl_node, void *data, int data_size, int (*cmp)(const void *, const void *), void (*free_data)(void*), int track, int *succes);
void avl_tree_remove(avl_tree_t *avl_tree, void *data, void (*free_data)(void*));
void avl_tree_free(avl_tree_t *avl_tree, void (*free_data)(void *));
int __avl_has_key(avl_node_t *avl_node, void *data, int (*cmp)(const void*, const void*));
int avl_has_key(avl_tree_t *avl_tree, void *data);
void avl_tree_print_inorder(avl_tree_t* avl_tree, void (*print_data)(void*));


#endif
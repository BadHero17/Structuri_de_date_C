#ifndef TREAP_H
#define TREAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

#define MAX_PRIORITY 1000

typedef struct treap_node_t treap_node_t;
struct  treap_node_t {
	/* informatia nodului */
	void *data;

	/* prioritatea nodului */
	int priority;

	/* cei doi copii, stang si drept */
	treap_node_t *left, *right;
};

typedef struct treap_tree_t treap_tree_t;
struct treap_tree_t {
	/* radacina arborelui */
	treap_node_t  *root;

	 /* dimensiunea datelor continute in fiecare nod */
	size_t data_size;

	/* functie de comparare a doua noduri */
	int	(*cmp)(void *key1, void *key2);
};

treap_tree_t* treap_create(size_t data_size, int (*cmp)(void*, void*));
void treap_free(treap_tree_t* treap, void (*free_data)(void *));
int priority(treap_node_t* node);
void treap_insert(treap_tree_t* treap, void* data);
void treap_delete(treap_tree_t* treap, void* data, void (*free_data)(void *));
void* get_key(treap_tree_t* treap, void* data);
void treap_ascending_nodes(treap_node_t* node, int* keys, int* num_keys);


#endif
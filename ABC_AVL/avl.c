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

// C implementation of the avl tree

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

unsigned char max(unsigned char a, unsigned char b) {
	return a < b ? b : a;
}

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
static avl_node_t *__avl_node_create(void *data, size_t data_size)
{
	avl_node_t *avl_node;  

	avl_node = (avl_node_t*)malloc(sizeof(*avl_node));

	DIE(avl_node == NULL, "avl_node malloc");

	avl_node->left = avl_node->right = NULL;

	avl_node->data = malloc(data_size);
	DIE(avl_node->data == NULL, "avl_node->data malloc");
	memcpy(avl_node->data, data, data_size);

	avl_node->height = 0;

	return avl_node;
}

/**
 * Alloc memory for a new avl
 * @data_size: size of the data contained by the avl's nodes
 * @cmp_f: pointer to a function used for sorting
 * @return: pointer to the newly created avl
 */
avl_tree_t *avl_tree_create(size_t data_size,
	int (*cmp_f)(const void *, const void *))
{
	avl_tree_t *avl_tree;

	avl_tree = (avl_tree_t*)malloc(sizeof(*avl_tree));
	DIE(avl_tree == NULL, "avl_tree malloc");

	avl_tree->root  = NULL;
	avl_tree->data_size = data_size;
	avl_tree->cmp   = cmp_f;
	avl_tree->size = 0;
	return avl_tree;
}

/*
* Helper function to rotate right (when we have to consecutive lefts with no right child)
*/

void turn_right(void *ptr, char side) {
	avl_node_t *x, *y, *temp;
	if (side == 2) {
		avl_tree_t *avl_tree = (avl_tree_t*)ptr;
		x = avl_tree->root;
		y = x->left;

		temp = y->right;
		y->right = x;
		avl_tree->root = y;
		x->left = temp;
	} else if (side == 1) {
		avl_node_t *parent = (avl_node_t*)ptr;
		x = parent->right;
		y = x->left;

		temp = y->right;
		y->right = x;
		parent->right = y;
		x->left = temp;
	} else {
		avl_node_t *parent = (avl_node_t*)ptr;
		x = parent->left;
		y = x->left;

		temp = y->right;
		y->right = x;
		parent->left = y;
		x->left = temp;
	}
	int a = -1, b = -1;
	if (x->left) a = x->left->height;
	if (x->right) b = x->right->height;
	x->height = max(a, b) + 1;
	a = -1; b = -1;
	if (y->left) a = y->left->height;
	if (y->right) b = y->right->height;
	y->height = max(a, b) + 1;
}

/*
* Helper function to rotate left (when we have to consecutive rights with no left child)
*/

void turn_left(void *ptr, char side) {
	avl_node_t *x, *y, *temp;
	if (side == 2) {
		avl_tree_t *avl_tree = (avl_tree_t*)ptr;
		x = avl_tree->root;
		y = x->right;

		temp = y->left;
		y->left = x;
		avl_tree->root = y;
		x->right = temp;
	} else if (side == 1) {
		avl_node_t *parent = (avl_node_t*)ptr;
		x = parent->right;
		y = x->right;

		temp = y->left;
		y->left = x;
		parent->right = y;
		x->right = temp;
	} else {
		avl_node_t *parent = (avl_node_t*)ptr;
		x = parent->left;
		y = x->right;

		temp = y->left;
		y->left = x;
		parent->left = y;
		x->right = temp;
	}
	int a = -1, b = -1;
	if (x->left) a = x->left->height;
	if (x->right) b = x->right->height;
	x->height = max(a, b) + 1;
	a = -1; b = -1;
	if (y->left) a = y->left->height;
	if (y->right) b = y->right->height;
	y->height = max(a, b) + 1;
}

/**
 * Insert a new element in a avl
 * @avl_tree: the avl where to insert the new element
 * @data: the data to be inserted in avl
 */

void __avl_tree_insert(avl_node_t *avl_node, void *data, int data_size, int (*cmp)(const void *key1, const void *key2), int *succes) {
	int rc = cmp(data, avl_node->data);
	if (rc < 0) {
		if (avl_node->left) {
			__avl_tree_insert(avl_node->left, data, data_size, cmp, succes);
		} else {
			avl_node_t *node = __avl_node_create(data, data_size);
			avl_node->left = node;
			if (avl_node->right) {
				avl_node->height = avl_node->right->height + 1;
			} else {
				avl_node->height = 1;
			}
			return;
		}
	} else if (rc > 0) {
		if (avl_node->right) {
			__avl_tree_insert(avl_node->right, data, data_size, cmp, succes);
		} else {
			avl_node_t *node = __avl_node_create(data, data_size);
			avl_node->right = node;
			if (avl_node->left) {
				avl_node->height = avl_node->left->height + 1;
			} else {
				avl_node->height = 1;
			}
			return;
		}
	} else if (rc == 0){
		*succes = 1;
		return;
	}
	int a = -1, b = -1;
	/*if (avl_node->left) a = avl_node->left->height;
	if (avl_node->right) b = avl_node->right->height;
	avl_node->height = max(a, b) + 1;*/
	if (rc < 0) {
		avl_node_t *curr = avl_node->left;
		a = -1;
		b = -1;
		if (curr->left) a = curr->left->height;
		if (curr->right) b = curr->right->height;
		if (b - a == -2) {
			a = -1;
			b = -1;
			if (curr->left->left) a = curr->left->left->height;
			if (curr->left->right) b = curr->left->right->height;
			if (b - a == -1) {
				turn_right(avl_node, 0);
			} else if (b - a == 1) {
				turn_left(curr, 0);
				turn_right(avl_node, 0);
			}
		} else if (b - a == 2) {
			a = -1;
			b = -1;
			if (curr->right->left) a = curr->right->left->height;
			if (curr->right->right) b = curr->right->right->height;
			if (b - a == -1) {
				turn_right(curr, 1);
				turn_left(avl_node, 0);
			} else if (b - a == 1) {
				turn_left(avl_node, 0);
			}
		}
	} else {
		avl_node_t *curr = avl_node->right;
		a = -1;
		b = -1;
		if (curr->left) a = curr->left->height;
		if (curr->right) b = curr->right->height;
		if (b - a == -2) {
			a = -1;
			b = -1;
			if (curr->left->left) a = curr->left->left->height;
			if (curr->left->right) b = curr->left->right->height;
			if (b - a == -1) {
				turn_right(avl_node, 1);
			} else if (b - a == 1) {
				turn_left(curr, 0);
				turn_right(avl_node, 1);
			}
		} else if (b - a == 2) {
			a = -1;
			b = -1;
			if (curr->right->left) a = curr->right->left->height;
			if (curr->right->right) b = curr->right->right->height;
			if (b - a == -1) {
				turn_right(curr, 1);
				turn_left(avl_node, 1);
			} else if (b - a == 1) {
				turn_left(avl_node, 1);
			}
		}
	}
	a = -1, b = -1;
	if (avl_node->left) a = avl_node->left->height;
	if (avl_node->right) b = avl_node->right->height;
	avl_node->height = max(a, b) + 1;
}

void avl_tree_insert(avl_tree_t *avl_tree, void *data) {
	int succes = 0;
	if (!avl_tree->root) {
		avl_tree->size++;
		avl_node_t *node = __avl_node_create(data, avl_tree->data_size);
		avl_tree->root = node;
		return;
	}
	__avl_tree_insert(avl_tree->root, data, avl_tree->data_size, avl_tree->cmp, &succes);
	avl_node_t *curr = avl_tree->root;
	int a = -1;
	int b = -1;
	if (curr->left) a = curr->left->height;
	if (curr->right) b = curr->right->height;
	if (b - a == -2) {
		a = -1;
		b = -1;
		if (curr->left->left) a = curr->left->left->height;
		if (curr->left->right) b = curr->left->right->height;
		if (b - a == -1) {
			turn_right(avl_tree, 2);
		} else if (b - a == 1) {
			turn_left(curr, 0);
			turn_right(avl_tree, 2);
		}
	} else if (b - a == 2) {
		a = -1;
		b = -1;
		if (curr->right->left) a = curr->right->left->height;
		if (curr->right->right) b = curr->right->right->height;
		if (b - a == -1) {
			turn_right(curr, 1);
			turn_left(avl_tree, 2);
		} else if (b - a == 1) {
			turn_left(avl_tree, 2);
		}
	}
	a = -1, b = -1;
	if (avl_tree->root->left) a = avl_tree->root->left->height;
	if (avl_tree->root->right) b = avl_tree->root->right->height;
	avl_tree->root->height = max(a, b) + 1;
	avl_tree->size += (1 - succes);
}

/**
 * Helper function to remove an element from a avl
 * @avl_node: the binary search subtree's root where to remove the element from
 * @data: the data that is contained by the node which has to be removed
 * @data_size: data size
 * @cmp: function used to compare the data contained by two nodes
 */

void __avl_tree_remove(avl_node_t *avl_node, void *data, int data_size, int (*cmp)(const void *, const void *), void (*free_data)(void*), int track, int *succes) {
	int rc = cmp(data, avl_node->data);
	if (track == 0) {
		if (rc < 0) {
			avl_node_t *curr = avl_node->left;
			if (!curr) {
				*succes = 1;
				return;
			}
			int rc2 = cmp(data, curr->data);
			if (rc2) {
				__avl_tree_remove(avl_node->left, data, data_size, cmp, free_data, 0, succes);
			} else {
				if (!curr->left&&!curr->right) {
					free_data(curr->data);
					free(curr);
					avl_node->left = NULL;
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else if (!curr->right) {
					avl_node->left = curr->left;
					free_data(curr->data);
					free(curr);
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else if (!curr->left) {
					avl_node->left = curr->right;
					free_data(curr->data);
					free(curr);
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else {
					avl_node_t *it = curr->left;
					if (it->right) {
						while (it->right->right) {
							it = it->right;
						}
						avl_node_t *temp = it->right;
						it->right = it->right->left;
						memcpy(curr->data, temp->data, data_size);
						free_data(temp->data);
						free(temp);
					} else {
						curr->left = it->left;
						memcpy(curr->data, it->data, data_size);
						free_data(it->data);
						free(it);
						printf("mata\n");
					}
					int a = -1, b = -1;
					if (curr->left) a = curr->left->height;
					if (curr->right) b = curr->right->height;
					curr->height = max(a, b) + 1;
					if (curr->left) __avl_tree_remove(curr->left, data, data_size, cmp, free_data, 1, succes);
				}
			}
		} else if (rc > 0) {
			avl_node_t *curr = avl_node->right;
			if (!curr) {
				*succes = 1;
				return;
			}
			int rc2 = cmp(data, curr->data);
			if (rc2) {
				__avl_tree_remove(avl_node->right, data, data_size, cmp, free_data, 0, succes);
			} else {
				if (!curr->left&&!curr->right) {
					free_data(curr->data);
					free(curr);
					avl_node->right = NULL;
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else if (!curr->right) {
					avl_node->right = curr->left;
					free_data(curr->data);
					free(curr);
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else if (!curr->left) {
					avl_node->right = curr->right;
					free_data(curr->data);
					free(curr);
					int a = -1, b = -1;
					if (avl_node->left) a = avl_node->left->height;
					if (avl_node->right) b = avl_node->right->height;
					avl_node->height = max(a, b) + 1;
					return;
				} else {
					avl_node_t *it = curr->left;
					if (it->right) {
						while (it->right->right) {
							it = it->right;
						}
						avl_node_t *temp = it->right;
						it->right = it->right->left;
						memcpy(curr->data, temp->data, data_size);
						free_data(temp->data);
						free(temp);
					} else {
						curr->left = it->left;
						memcpy(curr->data, it->data, data_size);
						free_data(it->data);
						free(it);
					}
					int a = -1, b = -1;
					if (curr->left) a = curr->left->height;
					if (curr->right) b = curr->right->height;
					curr->height = max(a, b) + 1;
					if (curr->left) __avl_tree_remove(curr->left, data, data_size, cmp, free_data, 1, succes);
				}
			}
		}
	} else {
		if (avl_node->right) {
			__avl_tree_remove(avl_node->left, data, data_size, cmp, free_data, 1, succes);
		}
	}
	// Code for rebalancing
	int a = -1, b = -1;
	/*if (avl_node->left) a = avl_node->left->height;
	if (avl_node->right) b = avl_node->right->height;
	avl_node->height = max(a, b) + 1;*/
	if (rc < 0) {
		avl_node_t *curr = avl_node->left;
		a = -1;
		b = -1;
		if (curr->left) a = curr->left->height;
		if (curr->right) b = curr->right->height;
		if (b - a == -2) {
			a = -1;
			b = -1;
			if (curr->left->left) a = curr->left->left->height;
			if (curr->left->right) b = curr->left->right->height;
			if (b - a == -1) {
				turn_right(avl_node, 0);
			} else if (b - a == 1) {
				turn_left(curr, 0);
				turn_right(avl_node, 0);
			}
		} else if (b - a == 2) {
			a = -1;
			b = -1;
			if (curr->right->left) a = curr->right->left->height;
			if (curr->right->right) b = curr->right->right->height;
			if (b - a == -1) {
				turn_right(curr, 1);
				turn_left(avl_node, 0);
			} else if (b - a == 1) {
				turn_left(avl_node, 0);
			}
		}
	} else {
		avl_node_t *curr = avl_node->right;
		a = -1;
		b = -1;
		if (curr->left) a = curr->left->height;
		if (curr->right) b = curr->right->height;
		if (b - a == -2) {
			a = -1;
			b = -1;
			if (curr->left->left) a = curr->left->left->height;
			if (curr->left->right) b = curr->left->right->height;
			if (b - a == -1) {
				turn_right(avl_node, 1);
			} else if (b - a == 1) {
				turn_left(curr, 0);
				turn_right(avl_node, 1);
			}
		} else if (b - a == 2) {
			a = -1;
			b = -1;
			if (curr->right->left) a = curr->right->left->height;
			if (curr->right->right) b = curr->right->right->height;
			if (b - a == -1) {
				turn_right(curr, 1);
				turn_left(avl_node, 1);
			} else if (b - a == 1) {
				turn_left(avl_node, 1);
			}
		}
	}
	a = -1, b = -1;
	if (avl_node->left) a = avl_node->left->height;
	if (avl_node->right) b = avl_node->right->height;
	avl_node->height = max(a, b) + 1;
}

/**
 * Remove an element from a avl
 * @avl_tree: the avl where to remove the element from
 * @data: the data that is contained by the node which has to be removed
 */
void avl_tree_remove(avl_tree_t *avl_tree, void *data, void (*free_data)(void*))
{
	int succes = 0;
	if (!avl_tree->root) {
		return;
	}
	if (!avl_tree->cmp(data, avl_tree->root->data)) {
		avl_node_t *curr = avl_tree->root;
		if (!curr->left&&!curr->right) {
			free_data(curr->data);
			free(curr);
			avl_tree->root = NULL;
		} else if (!curr->right) {
			avl_tree->root = curr->left;
			free_data(curr->data);
			free(curr);
		} else if (!curr->left) {
			avl_tree->root = curr->right;
			free_data(curr->data);
			free(curr);
		} else {
			avl_node_t *it = curr->left;
			if (it->right) {
				while (it->right->right) {
					it = it->right;
				}
				avl_node_t *temp = it->right;
				it->right = it->right->left;
				memcpy(curr->data, temp->data, avl_tree->data_size);
				free_data(temp->data);
				free(temp);
			} else {
				curr->left = it->left;
				memcpy(curr->data, it->data, avl_tree->data_size);
				free_data(it->data);
				free(it);
			}
			__avl_tree_remove(avl_tree->root->left, data, avl_tree->data_size, avl_tree->cmp, free_data, 1, &succes);
		}
	} else {
		__avl_tree_remove(avl_tree->root, data, avl_tree->data_size, avl_tree->cmp, free_data, 0, &succes);
	}

	avl_tree->size -= (1 - succes);

	// Rebalance

	avl_node_t *curr = avl_tree->root;
	int a = -1;
	int b = -1;
	if (curr->left) a = curr->left->height;
	if (curr->right) b = curr->right->height;
	if (b - a == -2) {
		a = -1;
		b = -1;
		if (curr->left->left) a = curr->left->left->height;
		if (curr->left->right) b = curr->left->right->height;
		if (b - a == -1) {
			turn_right(avl_tree, 2);
		} else if (b - a == 1) {
			turn_left(curr, 0);
			turn_right(avl_tree, 2);
		}
	} else if (b - a == 2) {
		a = -1;
		b = -1;
		if (curr->right->left) a = curr->right->left->height;
		if (curr->right->right) b = curr->right->right->height;
		if (b - a == -1) {
			turn_right(curr, 1);
			turn_left(avl_tree, 2);
		} else if (b - a == 1) {
			turn_left(avl_tree, 2);
		}
	}
	a = -1, b = -1;
	if (avl_tree->root->left) a = avl_tree->root->left->height;
	if (avl_tree->root->right) b = avl_tree->root->right->height;
	avl_tree->root->height = max(a, b) + 1;
}

/**
 * Free the left and the right subtree of a node, its data and itself
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
static void __avl_tree_free(avl_node_t *avl_node, void (*free_data)(void *))
{
	if (!avl_node)
		return;

	__avl_tree_free(avl_node->left, free_data);
	__avl_tree_free(avl_node->right, free_data);
	free_data(avl_node->data);
	free(avl_node);
}

/**
 * Free an avl
 * @avl_tree: the avl to be freed
 * @free_data: function used to free the data contained by a node
 */
void avl_tree_free(avl_tree_t *avl_tree, void (*free_data)(void *))
{
	__avl_tree_free(avl_tree->root, free_data);
	free(avl_tree);
}

static void __avl_tree_print_inorder(avl_node_t* avl_node,
	void (*print_data)(void*))
{
	if (!avl_node)
		return;
		
	__avl_tree_print_inorder(avl_node->left, print_data);
	print_data(avl_node->data);
	printf("%d\n", avl_node->height);
	__avl_tree_print_inorder(avl_node->right, print_data);
}

int __avl_has_key(avl_node_t *avl_node, void *data, int (*cmp)(const void*, const void*)) {
	int rc = cmp(data, avl_node->data);
	if (rc == 0) return 1;
	else if (rc < 0) {
		if (!avl_node->left) return 0;
		return __avl_has_key(avl_node->left, data, cmp);
	} else {
		if (!avl_node->right) return 0;
		return __avl_has_key(avl_node->right, data, cmp);
	}
}

int avl_has_key(avl_tree_t *avl_tree, void *data) {
	if (!avl_tree->root) return 0;
	return __avl_has_key(avl_tree->root, data, avl_tree->cmp);
}

/**
 * Print inorder a avl
 * @avl_tree: the avl to be printed
 * @print_data: function used to print the data contained by a node 
 */
void avl_tree_print_inorder(avl_tree_t* avl_tree, void (*print_data)(void*))
{
	__avl_tree_print_inorder(avl_tree->root, print_data);
}

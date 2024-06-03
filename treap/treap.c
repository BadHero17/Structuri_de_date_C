#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

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

/**
 * @brief Creeaza structura treap-ului
 * 
 * @param data_size Numarul de octeti pe care se scrie valoarea
 * @param cmp Functia de comparare a doua noduri
 * @return treap_tree_t* 
 */
treap_tree_t* treap_create(size_t data_size, int (*cmp)(void*, void*))
{
	// prioritatea va fi random
	srand(time(NULL));

	treap_tree_t *aux = calloc(1, sizeof(treap_tree_t));
	DIE(aux == NULL, "aux malloc");

	aux->data_size = data_size;
	aux->cmp = cmp;

	return aux;
}

/**
 * @brief Elibereaza memoria, plecand dintr-un nod
 * si mergand recursiv pe tot subarborele
 * 
 * @param node Nodul din care porneste
 */
static void __treap_node_free(treap_node_t** node, void (*free_data)(void *))
{
	free_data((*node)->data);
	__treap_node_free(&((*node)->right), free_data);
	__treap_node_free(&((*node)->left), free_data);
	free(*node);
	*node = NULL;
}

/**
 * @brief Elibereaza intreaga memoria folosita de treap
 * 
 * @param tree Arborele curent
 */
void treap_free(treap_tree_t* treap, void (*free_data)(void *))
{
	__treap_node_free(&(treap->root), free_data);
	free(treap);
}

/**
 * @brief Creeaza un nod
 * 
 * @param value Valoarea ce trebuie pusa in nod
 * @return treap_node_t
 */
static treap_node_t* __treap_node_create(void* data, size_t data_size)
{
	treap_node_t* node = malloc(sizeof(treap_node_t));
	DIE(!node, "malloc node");

	node->data = malloc(data_size);
	DIE(!node->data, "malloc node->data");

	memcpy(node->data, data, data_size);

	// Nodurile frunze au inaltimea 0
	node->priority = rand() % MAX_PRIORITY;
	node->left = NULL;
	node->right = NULL;

	return node;
}

static int max(int a, int b)
{
	if (a > b) {
		return a;
	}

	return b;
}

int priority(treap_node_t* node)
{
	// Nodurile NULL au prioritatea -1 pentru a pastra proprietatea de max-heap
	if (!node) {
		return -1;
	}

	return node->priority;
}

/* Rotire dreapta
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *      node  			    lson
 *     /    \			   /    \
 *   lson    y    --->    x    node
 *   /  \     		           /   \
 *  x   lrson                lrson  y
 */
static void __rotate_right(treap_node_t** node)
{
	treap_node_t *tmp = *node;
	*node = (*node)->left;
	tmp->left = (*node)->right;
	(*node)->right = tmp;
}

/* Rotire stanga
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *     node  			    rson
 *    /    \			   /    \
 *   x     rson    --->  node    y
 *         /   \		 /   \
 *       rlson  y       x   rlson
 */
static void __rotate_left(treap_node_t** node)
{
	treap_node_t *tmp = *node;
	*node = (*node)->right;
	tmp->right = (*node)->left;
	(*node)->left = tmp;
}

/**
 * @brief Inserare in treap pornind dintr-un nod dat
 * 
 * @param node Nodul radacina al subarborelui din parcurgerea recursiva
 * @param data Valoare de adaugat in treap
 * @param data_size Numarul de octeti pe care se scrie valoarea
 * @param cmp Functia de comparare pentru datele din treap
 */
static void __treap_insert(treap_node_t** node, void* data, int data_size, int (*cmp)(void*, void*))
{
	if (*node == NULL) {
		*node = __treap_node_create(data, data_size);
		return;
	}
	int rc = cmp(data, (*node)->data);
	if (rc < 0) {
		__treap_insert(&((*node)->left), data, data_size, cmp);
		if (priority(*node) < priority((*node)->left)) {
			__rotate_right(node);
		}
	} else if (rc > 0) {
		__treap_insert(&((*node)->right), data, data_size, cmp);
		if (priority(*node) < priority((*node)->right)) {
			__rotate_left(node);
		}
	}

}

/**
 * @brief Inserare in treap
 * 
 * @param treap Arborele curent
 * @param data Valoare de adaugat in treap
 */
void treap_insert(treap_tree_t* treap, void* data) {
	__treap_insert(&(treap->root), data, treap->data_size, treap->cmp);
}

/**
 * @brief Stergere din treap pornind dintr-un nod dat
 * 
 * @param node Nodul radacina al subarborelui din parcurgerea recursiva
 * @param data Valoare de sters din treap
 */
static void __treap_delete(treap_node_t** node, void* data, int (*cmp)(void*, void*), void (*free_data)(void *))
{
	int rc = cmp(data, (*node)->data);
	if (rc < 0) {
		__treap_delete(&((*node)->left), data, cmp, free_data);
	} else if (rc > 0) {
		__treap_delete(&((*node)->right), data, cmp, free_data);
	} else {
		if ((*node)->left == NULL && (*node)->right == NULL) {
			free_data((*node)->data);
			free(*node);
			*node = NULL;
		} else if ((*node)->right == NULL) {
			treap_node_t *tmp = (*node)->left;
			free_data((*node)->data);
			free(*node);
			*node = tmp;
		} else if ((*node)->left == NULL) {
			treap_node_t * tmp = (*node)->right;
			free_data((*node)->data);
			free(*node);
			*node = tmp;
		} else {
			if (priority((*node)->left) < priority((*node)->right)) {
				__rotate_left(node);
				__treap_delete(&((*node)->left), data, cmp, free_data);
			} else {
				__rotate_right(node);
				__treap_delete(&((*node)->right), data, cmp, free_data);
			}
		}
	}
}

/**
 * @brief Stergere din treap
 * 
 * @param treap Arborele curent
 * @param data Valoare de sters din treap
 */
void treap_delete(treap_tree_t* treap, void* data, void (*free_data)(void *)) {
	__treap_delete(&(treap->root), data, treap->cmp, free_data);
}

/**
 * @brief Gasirea unei valoari date in treap pornind dintr-un nod dat
 * 
 * @param node Nodul radacina al subarborelui din parcurgerea recursiva
 * @param data Valoare de sters in treap
 * @param cmp Functia de comparare pentru datele din treap
 * @return void* Valoarea gasita (data) sau NULL daca nu exista
 */
static void* __get_key(treap_node_t* node, void* data, int (*cmp)(void*, void*))
{
	if (!node) return NULL;
	int rc = cmp(data, node->data);
	if (rc < 0) {
		return __get_key(node->left, data, cmp);
	} else if (rc > 0) {
		return __get_key(node->right, data, cmp);
	} else return node->data;
}

/**
 * @brief Gasirea unei valoari date in treap
 * 
 * @param treap Arborele curent
 * @param data Valoare de sters din treap
 * @return void* Valoarea gasita (data) sau NULL daca nu exista
 */
void* get_key(treap_tree_t* treap, void* data) {
	return __get_key(treap->root, data, treap->cmp);
}

/**
 * @brief Obtinerea cheilor sortate crescator,
 * presupunand ca datele din noduri sunt int-uri
 * 
 * @param node Nodul curent in parcurgerea recursiva
 * @param keys Array-ul prin care se intorc cheile sortate crescator
 * @param num_keys Numarul de chei adaugate in array
 */
void treap_ascending_nodes(treap_node_t* node, int* keys, int* num_keys)
{
	if (!node) return;
	printf("%d\n", *(int*)node->data);
	treap_ascending_nodes(node->left, keys, num_keys);
	keys[*num_keys] = *(int*)node->data;
	*num_keys++;
	treap_ascending_nodes(node->right, keys, num_keys);
}

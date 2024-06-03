#ifndef BT_H
#define BT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_NODES 500
#define BUF_SIZ 512

#define DIE(assertion, call_description)  \
    do                                    \
    {                                     \
        if (assertion)                    \
        {                                 \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__);  \
            perror(call_description);     \
            exit(errno);                  \
        }                                 \
    } while (0)

/* Helper data structure definitions */
typedef struct queue_t queue_t;
struct queue_t
{
    /* Dimensiunea maxima a cozii */
    unsigned int max_size;
    /* Dimensiunea cozii */
    unsigned int size;
    /* Dimensiunea in octeti a tipului de date stocat in coada */
    unsigned int data_size;
    /* Indexul de la care se vor efectua operatiile de front si dequeue */
    unsigned int read_idx;
    /* Indexul de la care se vor efectua operatiile de enqueue */
    unsigned int write_idx;
    /* Bufferul ce stocheaza elementele cozii */
    void **buff;
};

typedef struct b_node_t b_node_t;
struct b_node_t
{
    /* left child */
    b_node_t *left;
    /* right child */
    b_node_t *right;

    /* data contained by the node */
    void *data;
};

typedef struct b_tree_t b_tree_t;
struct b_tree_t
{
    /* root of the tree */
    b_node_t *root;

    /* size of the data contained by the nodes */
    size_t data_size;
};

queue_t * q_create(unsigned int data_size, unsigned int max_size);
unsigned int q_get_size(queue_t *q);
unsigned int q_is_empty(queue_t *q);
void *q_front(queue_t *q);
int q_dequeue(queue_t *q);
int q_enqueue(queue_t *q, void *new_data);
void q_clear(queue_t *q);
void q_free(queue_t *q);
b_tree_t * b_tree_create(size_t data_size);
void b_tree_insert(b_tree_t *b_tree, void *data);
void b_tree_print_preorder(b_tree_t *b_tree, void (*print_data)(void *));
void b_tree_print_inorder(b_tree_t *b_tree, void (*print_data)(void *));
void b_tree_print_postorder(b_tree_t *b_tree, void (*print_data)(void *));
void b_tree_free(b_tree_t *b_tree, void (*free_data)(void *));

#endif
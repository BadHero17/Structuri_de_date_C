#ifndef QUEUE_STACK_HEADER
#define QUEUE_STACK_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define MAX_STRING_SIZE 4096

typedef struct queue_t queue_t; 
struct queue_t {
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

typedef struct ll_node_t ll_node_t;
struct ll_node_t {
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t {
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

queue_t * q_create(unsigned int data_size, unsigned int max_size);
unsigned int q_get_size(queue_t *q);
unsigned int q_is_empty(queue_t *q);
void* q_front(queue_t *q);
int q_dequeue(queue_t *q);
int q_enqueue(queue_t *q, void *new_data);
void q_clear(queue_t *q);
void q_free(queue_t *q);

struct Stack {
    linked_list_t *list;
};

void init_stack(struct Stack *stack, unsigned int data_size);
int get_size_stack(struct Stack *stack);
int is_empty_stack(struct Stack *stack);
void* peek_stack(struct Stack *stack);
void pop_stack(struct Stack *stack);
void push_stack(struct Stack *stack, void *new_data);
void clear_stack(struct Stack *stack);
void purge_stack(struct Stack *stack);

#endif
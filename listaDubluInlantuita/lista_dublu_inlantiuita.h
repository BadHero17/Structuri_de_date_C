#ifndef LIST_HEADER
#define LIST_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dll_node_t {
	void* data; 
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t {
	dll_node_t* head;
	unsigned int data_size;
	unsigned int size;
} doubly_linked_list_t;

#define DIE(assertion, call_description)				\
	do {								\
		if (!assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)


doubly_linked_list_t* dll_create(unsigned int data_size);
dll_node_t* dll_get_nth_node(doubly_linked_list_t* list, unsigned int n);
void dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data);
dll_node_t* dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n);
unsigned int dll_get_size(doubly_linked_list_t* list);
void dll_free(doubly_linked_list_t** pp_list);
void dll_print_int(doubly_linked_list_t* list);
doubly_linked_list_t* sum_lists(doubly_linked_list_t* a, doubly_linked_list_t* b);


#endif
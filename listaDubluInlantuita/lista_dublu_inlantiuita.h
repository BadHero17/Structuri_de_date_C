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
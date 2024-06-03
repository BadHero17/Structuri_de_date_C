#include "Queue_Stack.h"

// La stack am decis sa utilizez liste inlantuite pentru implementare

static linked_list_t* ll_create(unsigned int data_size) {
    linked_list_t* ll;

    ll = malloc(sizeof(linked_list_t));
    DIE(ll == NULL, "linked_list malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}


static void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data) {
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (list == NULL) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    } else if (n < 0) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(ll_node_t));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

static ll_node_t* ll_remove_nth_node(linked_list_t* list, unsigned int n) {
    ll_node_t *prev, *curr;

    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}


static unsigned int ll_get_size(linked_list_t* list) {
    if (list == NULL) {
        return -1;
    }

    return list->size;
}


static void ll_free(linked_list_t** pp_list) {
    ll_node_t* currNode;

    if (pp_list == NULL || *pp_list == NULL) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        free(currNode);
    }

    free(*pp_list);
    *pp_list = NULL;
}

static void ll_print_int(linked_list_t* list) {
    ll_node_t* curr;

    if (list == NULL) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

static void ll_print_string(linked_list_t* list) {
    ll_node_t* curr;

    if (list == NULL) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}


/* ---------------------------------- Stack implementation ----------------------------------------*/

void init_stack(struct Stack *stack, unsigned int data_size) {
    stack->list = ll_create(data_size);
}

int get_size_stack(struct Stack *stack) {
    return stack->list->size;
}

int is_empty_stack(struct Stack *stack) {
    if(!stack->list->size) return 1;
    return 0;
}

void* peek_stack(struct Stack *stack) {
    if(!stack->list->size) return NULL;
    else return stack->list->head->data;
}

void pop_stack(struct Stack *stack) {
    if(stack->list->size) {
        ll_remove_nth_node(stack->list, 0);
    }
}

void push_stack(struct Stack *stack, void *new_data) {
    ll_add_nth_node(stack->list, 0, new_data);
}

void clear_stack(struct Stack *stack) {
    while (stack->list->size) {
        pop_stack(stack);
    }
}

void purge_stack(struct Stack *stack) {
    ll_free(&(stack->list));
}

/*
* Functia initializeaza o coada de elemente de marime data_size bytes
*/
queue_t * q_create(unsigned int data_size, unsigned int max_size) {
    queue_t *current = calloc(1, sizeof(queue_t));
    DIE(current == NULL, "Failed allocation");
    current->buff = calloc(max_size, sizeof(void*));
    DIE(current->buff == NULL, "Failed allocation");
    for (int i = 0; i < max_size; i++) {
        current->buff[i] = calloc(1, data_size);
        DIE(current->buff[i] == NULL, "Failed allocation");
    }
    current->max_size = max_size;
    current->data_size = data_size;
	return current;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int q_get_size(queue_t *q) {
	return q->size;
}

/*
 * Functia intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int q_is_empty(queue_t *q) {
	if (!q->size) return 1;
	return 0;
}

/* 
 * Functia intoarce primul element din coada, fara sa il elimine.
 */ 
void* q_front(queue_t *q) {
    if (q->size) {
        return q->buff[q->read_idx];
    }
	return NULL;
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
int q_dequeue(queue_t *q) {
	if (!q_is_empty(q)) {
        q->read_idx++;
        q->read_idx %= q->max_size;
        q->size--;
        return 1;
    }
	return 0;
}

/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
int q_enqueue(queue_t *q, void *new_data) {
    if (q->size != q->max_size) {
        memcpy(q->buff[q->write_idx], new_data, q->data_size);
        q->write_idx = (q->write_idx + 1) % q->max_size;
        q->size++;
        return 1;
    }
	return 0;
}

/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */
void q_clear(queue_t *q) {
	q->size = 0;
    q->read_idx = 0;
    q->write_idx = 0;
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void q_free(queue_t *q) {
    for(int i = 0; i < q->max_size; i++) {
        free(q->buff[i]);
    }
	free(q->buff);
    free(q);
}

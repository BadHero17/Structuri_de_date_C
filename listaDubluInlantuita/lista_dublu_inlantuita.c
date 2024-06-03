/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii list).
 */
doubly_linked_list_t* dll_create(unsigned int data_size) {
	doubly_linked_list_t* current = calloc(1, sizeof(doubly_linked_list_t));
	DIE(current, "Failed allocation");
	current->data_size = data_size;
	return current;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo.
 */
dll_node_t* dll_get_nth_node(doubly_linked_list_t* list, unsigned int n) {
	n %= list->size;
	dll_node_t* iterator = list->head;
	for (int i = 0; i < n; i++) {
		iterator = iterator->next;
	}
	return iterator;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 */
void dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data) {
	if (n == 0 || list->size == 0) {
		dll_node_t* new = calloc(1, sizeof(dll_node_t));
		DIE(new, "Failed allocation");
		new->data = calloc(1, list->data_size);
		DIE(new->data, "Failed allocation");
		new->next = list->head;
		if (list->head) list->head->prev = new;
		list->head = new;
		memcpy(new->data, data, list->data_size);
	} else {
		if (n >= list->size) n = list->size;
		dll_node_t *new = calloc(1, sizeof(dll_node_t));
		DIE(new, "Failed allocation");
		new->data = calloc(1, list->data_size);
		DIE(new->data, "Failed allocation");
		dll_node_t *last = dll_get_nth_node(list, n - 1);
		if (last->next) last->next->prev = new;
		new->next = last->next;
		new->prev = last;
		last->next = new;
		memcpy(new->data, data, list->data_size);
	}
	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 */
dll_node_t* dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n) {
	if (n > list->size) n = list->size - 1;
	if (n == 0) {
		list->head = list->head->next;
		list->head->prev = NULL;
	} else if (n == list->size - 1) {
		dll_node_t *target = dll_get_nth_node(list, n);
		target->prev->next = NULL;
	} else {
		dll_node_t *target = dll_get_nth_node(list, n);
		target->prev->next = target->next;
		target->next->prev = target->prev;
	}
	list->size--;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int dll_get_size(doubly_linked_list_t* list) {
	return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void dll_free(doubly_linked_list_t** pp_list) {
	dll_node_t *iterator = (*pp_list)->head;
	dll_node_t *temp = iterator->next;
	for(int i = 0; i < (*pp_list)->size; i++) {
		free(iterator->data);
		free(iterator);
		iterator = temp;
		if (temp) temp = temp->next;
	}
	free(*pp_list);
	*pp_list = NULL;
}

/*
 * Functie ce strie o lista in care stim ca pastram elemente de tip int
 */
void dll_print_int(doubly_linked_list_t* list) {
	dll_node_t *iterator = list->head;
	for(int i = 0; i < list->size; i++) {
		printf("%d ", *(int*)iterator->data);
		iterator = iterator->next;
	}
	printf("\n");
}

/*
 * Procedura primeste doua liste simplu inlantuite, A si B, ale căror noduri
 * stochează în ordine inversă cifrele câte unui număr natural reprezentat
 * în baza 10 (primul nod al unei liste stochează cea mai puțin semnificativă
 * cifră). De exemplu: lista A: 0 -> 0 -> 3 contine numarul 300.
 * Creați o nouă listă simplu înlănțuită, C, care stochează suma celor două
 * numere si returnati lista.
*/
doubly_linked_list_t* sum_lists(doubly_linked_list_t* a, doubly_linked_list_t* b) {
	doubly_linked_list_t *c = dll_create(sizeof(int));
	DIE(c, "Failed allocation");
	dll_node_t *ita = a->head, *itb = b->head;
	int carry = 0;
	while (ita && itb) {
		int digit = (*(int*)ita->data + *(int*)itb->data + carry) % 10;
		carry = (*(int*)ita->data + *(int*)itb->data + carry) / 10;
		ita = ita->next; itb = itb->next;
		dll_add_nth_node(c, c->size, &digit);
	}
	while (ita) {
		int digit = (*(int*)ita->data + carry) % 10;
		carry = (*(int*)ita->data + carry) / 10;
		ita = ita->next;
		dll_add_nth_node(c, c->size, &digit);
	}
	while (itb) {
		int digit = (*(int*)itb->data + carry) % 10;
		carry = (*(int*)itb->data + carry) / 10;
		itb = itb->next;
		dll_add_nth_node(c, c->size, &digit);
	}
	if (carry) {
		dll_add_nth_node(c, c->size, &carry);
	}
	return c;
}

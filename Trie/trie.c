trie_node_t* trie_create_node(trie_t *trie) {
    trie_node_t *current = calloc(1, sizeof(trie_node_t));
    DIE(current == NULL, "trie_node_t calloc");

    current->children = calloc(trie->alphabet_size, sizeof(trie_node_t*));
    DIE(current->children == NULL, "trie_node_t->children calloc");

    return current;
}

trie_t* trie_create(int data_size, int alphabet_size, char* alphabet, void (*free_value_cb)(void*)) {
    trie_t *current = calloc(1, sizeof(trie_t));
    DIE(current == NULL, "trie calloc");

    current->data_size = data_size;
    current->alphabet_size = alphabet_size;
    current->alphabet = alphabet;

    current->free_value_cb = free_value_cb;

    current->root = trie_create_node(current);
    current->nNodes = 1;
    return current;
}

void trie_insert(trie_t* trie, char* key, void* value) {
    trie_node_t *iterator = trie->root;
    while (*key) {
        if (!iterator->children[*key - 'a']) {
            iterator->children[*key- 'a'] = trie_create_node(trie);
            iterator->n_children++;
            trie->nNodes++;
        }
        iterator = iterator->children[*key - 'a'];
        key++;
    }
    iterator->value = calloc(1, trie->data_size);
    memcpy(iterator->value, value, trie->data_size);
    iterator->end_of_word = 1;
}

void* trie_search(trie_t* trie, char* key) {
    trie_node_t *iterator = trie->root;
    while (*key) {
        if (iterator->children[*key - 'a']) {
            iterator = iterator->children[*key - 'a'];
            key++;
        } else {
            return NULL;
        }
    }
    if (iterator->end_of_word == 0) return NULL;
    return iterator->value;
}

int __trie_remove(trie_node_t *trie_node, trie_t *trie, char *key) {
    if (!*key) {
        if (trie_node->end_of_word == 1) {
            if (trie_node->n_children) {
                trie_node->end_of_word = 0;
                return 0;
            } else {
                trie->free_value_cb(trie_node->value);
                free(trie_node->children);
                free(trie_node);
                trie->nNodes--;
                return 1;
            }
        } else return 0;
    } else {
        int test = 0;
        if (trie_node->children[*key - 'a']) {
            test = __trie_remove(trie_node->children[*key - 'a'], trie, key + 1);
        }
        if (test) {
            trie_node->children[*key - 'a'] = NULL;
            trie_node->n_children--;
            if (trie_node->n_children == 0) {
                free(trie_node->children);
                free(trie_node);
                trie->nNodes--;
                return 1;
            } else return 0;
        } else return 0;
    }
}

void trie_remove(trie_t* trie, char* key) {
    __trie_remove(trie->root, trie, key);
}

void __trie_free(trie_node_t *trie_node, void (*free_value_cb)(void*), int alphabet_size) {
    for (int i = 0; i < alphabet_size; i++) {
        if (trie_node->children[i]) {
            __trie_free(trie_node->children[i], free_value_cb, alphabet_size);
        }
    }
    if (trie_node->end_of_word) {
        free_value_cb(trie_node->value);
    }
    free(trie_node->children);
    free(trie_node);
}

void trie_free(trie_t** pTrie) {
    __trie_free((*pTrie)->root, (*pTrie)->free_value_cb, (*pTrie)->alphabet_size);
    free(*pTrie);
    *pTrie = NULL;
}   

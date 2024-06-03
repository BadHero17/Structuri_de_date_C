queue_t * q_create(unsigned int data_size, unsigned int max_size)
{
    queue_t *q = calloc(1, sizeof(*q));
    DIE(!q, "calloc queue failed");

    q->data_size = data_size;
    q->max_size = max_size;

    q->buff = malloc(max_size * sizeof(*q->buff));
    DIE(!q->buff, "malloc buffer failed");

    return q;
}

unsigned int q_get_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

unsigned int q_is_empty(queue_t *q)
{
    return !q ? 1 : !q->size;
}

void *q_front(queue_t *q)
{
    if (!q || !q->size)
        return NULL;

    return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
    if (!q || !q->size)
        return 0;

    free(q->buff[q->read_idx]);

    q->read_idx = (q->read_idx + 1) % q->max_size;
    --q->size;
    return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
    void *data;
    if (!q || q->size == q->max_size)
        return 0;

    data = malloc(q->data_size);
    DIE(!data, "malloc data failed");
    memcpy(data, new_data, q->data_size);

    q->buff[q->write_idx] = data;
    q->write_idx = (q->write_idx + 1) % q->max_size;
    ++q->size;

    return 1;
}

void q_clear(queue_t *q)
{
    unsigned int i;
    if (!q || !q->size)
        return;

    for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
        free(q->buff[i]);

    q->read_idx = 0;
    q->write_idx = 0;
    q->size = 0;
}

void q_free(queue_t *q)
{
    if (!q)
        return;

    q_clear(q);
    free(q->buff);
    free(q);
}

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
static b_node_t *__b_node_create(void *data, size_t data_size)
{
    b_node_t *b_node;

    b_node = malloc(sizeof(*b_node));
    DIE(b_node == NULL, "b_node malloc");

    b_node->left = b_node->right = NULL;

    b_node->data = malloc(data_size);
    DIE(b_node->data == NULL, "b_node->data malloc");
    memcpy(b_node->data, data, data_size);

    return b_node;
}

b_tree_t * b_tree_create(size_t data_size) {
    b_tree_t *current = calloc(1, sizeof(b_tree_t));
    DIE(current == NULL, "Failed allocation");
    current->data_size = data_size;
    return current;
}

void b_tree_insert(b_tree_t *b_tree, void *data) {
    queue_t *q;
    b_node_t *b_node, *b_node_tmp;
    //b_node_t **b_node_tmp_addr;

    b_node = __b_node_create(data, b_tree->data_size);

    if (!b_tree->root)
    {
        b_tree->root = b_node;
        return;
    }

    q = q_create(sizeof(b_node_t *), MAX_NODES);

    q_enqueue(q, &b_tree->root);
    while (q_get_size(q)) {
    	b_node_tmp = *(b_node_t**)q_front(q);
    	q_dequeue(q);
    	if (b_node_tmp->left) {
    		q_enqueue(q, &b_node_tmp->left);
    	} else {
    		b_node_tmp->left = b_node;
    		break;
    	}
    	if (b_node_tmp->right) {
    		q_enqueue(q, &b_node_tmp->right);
    	} else {
    		b_node_tmp->right = b_node;
    		break;
    	}
    }
    q_free(q);
}

static void __b_tree_print_preorder(b_node_t *b_node, void (*print_data)(void *)) {
    if (!b_node)
        return;
    print_data(b_node->data);
    __b_tree_print_preorder(b_node->left, print_data);
    __b_tree_print_preorder(b_node->right, print_data);
}

void b_tree_print_preorder(b_tree_t *b_tree, void (*print_data)(void *)) {
    __b_tree_print_preorder(b_tree->root, print_data);
    printf("\n");
}

static void __b_tree_print_inorder(b_node_t *b_node, void (*print_data)(void *)) {
    if (!b_node)
        return;

    __b_tree_print_preorder(b_node->left, print_data);
    print_data(b_node->data);
    __b_tree_print_preorder(b_node->right, print_data);
}

void b_tree_print_inorder(b_tree_t *b_tree, void (*print_data)(void *)) {
    __b_tree_print_inorder(b_tree->root, print_data);
    printf("\n");
}

static void __b_tree_print_postorder(b_node_t *b_node, void (*print_data)(void *)) {
    if (!b_node)
        return;

    __b_tree_print_preorder(b_node->left, print_data);
    __b_tree_print_preorder(b_node->right, print_data);
    print_data(b_node->data);
}

void b_tree_print_postorder(b_tree_t *b_tree, void (*print_data)(void *)) {
    __b_tree_print_postorder(b_tree->root, print_data);
    printf("\n");
}

/**
 * Free the left and the right subtree of a node, its data and itself
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
static void __b_tree_free(b_node_t *b_node, void (*free_data)(void *)) {
    if (!b_node)
        return;
    __b_tree_free(b_node->left, free_data);
    __b_tree_free(b_node->right, free_data);
    free_data(b_node->data);
    free(b_node);
}

void b_tree_free(b_tree_t *b_tree, void (*free_data)(void *)) {
    __b_tree_free(b_tree->root, free_data);
    free(b_tree);
}

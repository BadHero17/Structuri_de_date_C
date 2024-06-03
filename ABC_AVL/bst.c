// Simple implementation of the bst, for the self balancing version,
// check the avl.c file

typedef struct bst_node_t bst_node_t;
struct  bst_node_t {
	/* left child */
	bst_node_t *left;

	/* right child */
	bst_node_t *right;

	/* data contained by the node */
	void *data;
};

typedef struct bst_tree_t bst_tree_t;
struct bst_tree_t {
	/* root of the tree */
	bst_node_t  *root;

	 /* size of the data contained by the nodes */
	size_t data_size;

	/* function used for sorting the keys */
	int	(*cmp)(const void *key1, const void *key2);
};

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
static bst_node_t *__bst_node_create(void *data, size_t data_size)
{
	bst_node_t *bst_node;  

	bst_node = malloc(sizeof(*bst_node));

	DIE(bst_node == NULL, "bst_node malloc");

	bst_node->left = bst_node->right = NULL;

	bst_node->data = malloc(data_size);
	DIE(bst_node->data == NULL, "bst_node->data malloc");
	memcpy(bst_node->data, data, data_size);

	return bst_node;
}

/**
 * Alloc memory for a new BST
 * @data_size: size of the data contained by the BST's nodes
 * @cmp_f: pointer to a function used for sorting
 * @return: pointer to the newly created BST
 */
bst_tree_t *bst_tree_create(size_t data_size,
	int (*cmp_f)(const void *, const void *))
{
	bst_tree_t *bst_tree;

	bst_tree = malloc(sizeof(*bst_tree));
	DIE(bst_tree == NULL, "bst_tree malloc");

	bst_tree->root  = NULL;
	bst_tree->data_size = data_size;
	bst_tree->cmp   = cmp_f;

	return bst_tree;
}

/**
 * Insert a new element in a BST
 * @bst_tree: the BST where to insert the new element
 * @data: the data to be inserted in BST
 */
void bst_tree_insert(bst_tree_t *bst_tree, void *data)
{
	int rc;
	bst_node_t *root	= bst_tree->root;
	bst_node_t *parent	= NULL;
	bst_node_t *node	= __bst_node_create(data, bst_tree->data_size);

	int last_turn = -1;
	while (root) {
		parent = root;
		rc = bst_tree->cmp(root->data, data);
		if (rc > 0) {
			last_turn = 0;
			root = root->left;
		} else if (rc < 0) {
			last_turn = 1;
			root = root->right;
		} else return;
	}
	if (last_turn == 1) {
		parent->right = node;
	} else if (last_turn == 0) {
		parent->left = node;
	} else bst_tree->root = node;

}

/**
 * Helper function to remove an element from a BST
 * @bst_node: the binary search subtree's root where to remove the element from
 * @data: the data that is contained by the node which has to be removed
 * @data_size: data size
 * @cmp: function used to compare the data contained by two nodes
 */
static bst_node_t *__bst_tree_remove(bst_node_t *bst_node,
	void *data, size_t data_size,
	int (*cmp)(const void *, const void *))
{
	int rc;
	bst_node_t *tmp, *parent;

	if (!bst_node)
		return NULL;

	rc = cmp(data, bst_node->data);

	if (rc < 0) {
		bst_node->left = __bst_tree_remove(bst_node->left, data, data_size, cmp);
	} else if (rc > 0) {
		bst_node->right = __bst_tree_remove(bst_node->right, data, data_size, cmp);
	} else {
		if (bst_node->left == NULL && bst_node->right == NULL) {
			free(bst_node->data);
			free(bst_node);
			return NULL;
		} else if (bst_node->left == NULL) {
			tmp = bst_node->right;
			free(bst_node->data);
			free(bst_node);
			return tmp;
		} else if (bst_node->right == NULL) {
			tmp = bst_node->left;
			free(bst_node->data);
			free(bst_node);
			return tmp;
		} else {
			tmp = bst_node->left;
			parent = bst_node;
			while (tmp->right) {
				parent = tmp;
				tmp = tmp->right;
			}
			parent->right = tmp->left;
			memcpy(bst_node->data, tmp->data, data_size);
			free(tmp->data);
			free(tmp);
			return bst_node;
		}
	}

	return bst_node;
}

/**
 * Remove an element from a BST
 * @bst_tree: the BST where to remove the element from
 * @data: the data that is contained by the node which has to be removed
 */
void bst_tree_remove(bst_tree_t *bst_tree, void *data)
{
	bst_tree->root = __bst_tree_remove(bst_tree->root, data,
		bst_tree->data_size, bst_tree->cmp);
}

/**
 * Free the left and the right subtree of a node, its data and itself
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
static void __bst_tree_free(bst_node_t *bst_node, void (*free_data)(void *))
{
	if (!bst_node)
		return;

	__bst_tree_free(bst_node->left, free_data);
	__bst_tree_free(bst_node->right, free_data);
	free_data(bst_node->data);
	free(bst_node);
}

/**
 * Free a BST
 * @bst_tree: the BST to be freed
 * @free_data: function used to free the data contained by a node
 */
void bst_tree_free(bst_tree_t *bst_tree, void (*free_data)(void *))
{
	__bst_tree_free(bst_tree->root, free_data);
	free(bst_tree);
}

static void __bst_tree_print_inorder(bst_node_t* bst_node,
	void (*print_data)(void*))
{
	if (!bst_node)
		return;
		
	__bst_tree_print_inorder(bst_node->left, print_data);
	print_data(bst_node->data);
	__bst_tree_print_inorder(bst_node->right, print_data);
}

/**
 * Print inorder a BST
 * @bst_tree: the BST to be printed
 * @print_data: function used to print the data contained by a node 
 */
void bst_tree_print_inorder(bst_tree_t* bst_tree, void (*print_data)(void*))
{
	__bst_tree_print_inorder(bst_tree->root, print_data);
}

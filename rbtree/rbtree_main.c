#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"
#include "rbtree_print.h"

node * init_node(int val, color_t c) 
{
	node *new_node = malloc(sizeof(node));
	new_node->value = val;
	new_node->color = c;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->parent = NULL;
	return new_node;
}

int main() 
{
	node *tree = NULL;
	node *n ;

	/* function pointer: insert_func */
	node * (*insert_func)(node *root, node* new_node);
	int num_node, option;

	printf("BST(1) or RBTree(2)? ");
	scanf("%d", &option);
	if (option == 1)
		insert_func = btree_insert;
	else if (option == 2) 
		insert_func = rbtree_insert;
	else {
		printf("Error: wrong tree insertion option\n");
		exit(1);
	}

	printf("Enter the number of nodes (1-10): " );
	scanf("%d", &num_node);
	if ( num_node < 1 || num_node > 10 ) {
		printf("Error: wrong number of nodes\n");
		exit(1);
	}

	for (int i=1;i<=num_node;i++)
		tree = insert_func(tree, init_node(i,RED));
	print_t(tree);

	printf("Inorder: ");
	print_inorder(tree);
	printf("\n");

	btree_free(tree);

	return 0;
}
#ifndef RB_TREE_H
#define RB_TREE_H

typedef enum {
	RED = 0,
	BLACK
} color_t ;

typedef struct node {
	int value;
	color_t color;
	struct node *left, *right, *parent;
} node ;

node* grandparent(node* n);
node* uncle(node* n);
void btree_free(node *root);
node* rotate_left(node* n, node* root);
node* rotate_right(node* n, node* root);
node* insert_case1(node* n, node* root);
node* insert_case2(node* n, node* root);
node* insert_case3(node* n, node* root);
node* insert_case4(node* n, node* root);
node* insert_case5(node* n, node* root);
node* btree_insert(node *root, node* n);
node* rbtree_insert(node *root, node* n);


#endif // RB_TREE_H
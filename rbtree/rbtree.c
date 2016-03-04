#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "rbtree.h"


void btree_free(node *root) 
{
	if ( root != NULL ) {
		btree_free(root->left);
		btree_free(root->right);
		free(root);
	}
}

node* grandparent(node* n){
	if((n!=NULL)&(n->parent!=NULL)){
		return n->parent->parent;
	}
	else return NULL;
}

node* uncle(node* n){
	node* g = grandparent(n);
	if(g==NULL) return NULL;
	else if(n->parent == g->right) return g->left;
	else return g->right;
}

node* rotate_left(node* n, node* r){
	if(n!=NULL && r!=NULL){
		if(n->right!=NULL){
			node* m = n->right;
			if(n == r){
				r = m;
				m->parent = NULL;
			}
			else{
				if(n==n->parent->left) n->parent->left = m;
				else if(n==n->parent->right) n->parent->right = m;
				m->parent = n->parent;
			}
			if(m->left!=NULL) m->left->parent = n;
			n->right = m->left;
			m-> left = n;
			n->parent = m;
		}
	}
	return r;
}

node* rotate_right(node* n, node* r){
	if(n!=NULL && r!=NULL){
		if(n->left!=NULL){
			node* m = n->left;
			if(n == r){
				r = m;
				m->parent = NULL;
			}
			else{
				if(n==n->parent->left) n->parent->left = m;
				else if(n==n->parent->right) n->parent->right = m;
				m->parent = n->parent;
			}
			if(m->left!=NULL) m->right->parent = n;
			n->left = m->right;
			m-> right = n;
			n->parent = m;
		}
	}
	return r;
}

node* insert_case1(node* n, node* r){
	if (n->parent == NULL){
  		n->color = BLACK;
  		return r;
	}
  	else return insert_case2(n, r);
}

node* insert_case2(node* n, node* r){
	if (n->parent->color == BLACK){
		return r;
	}else return insert_case3(n, r);
}

node* insert_case3(node* n, node* r){
	node* g;
	node* u = uncle(n);
	if((u!=NULL)&&(u->color==RED)){
		n->parent->color = BLACK;
		u->color = BLACK;
		g = grandparent(n);
		g->color = RED;
		return insert_case1(g, r);
	}
	else{return insert_case4(n, r);}
}

node* insert_case4(node* n, node* r){
	node* g = grandparent(n);
	if((n->parent==g->left)&(n==n->parent->right)){
		r = rotate_left(n->parent, r);
		n = n->left;
	}
	else if((n->parent==g->right)&(n==n->parent->left)){
		r = rotate_right(n->parent, r);
		n = n->right;
	}
	return insert_case5(n, r);
}

node* insert_case5(node* n, node* r){
	node* g = grandparent(n);
	n->parent->color = BLACK;
	g->color = RED;
	if(n == n->parent->left) r = rotate_right(g, r);
	else r = rotate_left(g, r);
	return r;
}


node* btree_insert(node *root, node* n) 
{
	if(n!=NULL){
		if (root == NULL) {
			root = n;
		}
		else if(root->value == n->value){
			printf("Error: insert an exsited node\n");
			exit(1);
		}
		else if(n->value<root->value){
			if(root->left==NULL) n->parent = root;
			root->left = btree_insert(root->left, n);
		}
		else {
			if(root->right==NULL) n->parent = root;
			root->right = btree_insert(root->right, n);
		}
	}
	return root; 
}


node* rbtree_insert(node *root, node* n) 
{
	root = btree_insert(root, n);
	root = insert_case1(n, root);
	return root; // delete this line to start if needed
}



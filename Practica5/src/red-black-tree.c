/**
 *
 * Red-black tree implementation. 
 * 
 * Binary search trees work best when they are balanced or the path length from
 * root to any leaf is within some bounds. The red-black tree algorithm is a
 * method for balancing trees. The name derives from the fact that each node is
 * colored red or black, and the color of the node is instrumental in
 * determining the balance of the tree. During insert and delete operations,
 * nodes may be rotated to maintain tree balance. Both average and worst-case
 * search time is O(lg n).
 *
 * This implementation is original from John Morris, University of Auckland, at
 * the following link
 *
 * http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.htm
 *
 * and has been adapted here by Lluis Garrido, June 2018.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "red-black-tree.h"


void copyTreeRecursive(node *original, node *copia) {
    
    if (original->left== NIL){
        copia->left=NIL;
    }
    else{
        node *left_copia;
        left_copia = malloc(sizeof(node)); //Reservem espai pel node copia
        copia->left = left_copia;
        copyTreeRecursive(original->left, copia->left); //Seguim pel node->left de cada arbre
    }
    
    //COPIEM LA INFROMACIÓ DEL NODE
    node_data *copia_nodedata = malloc(sizeof(node_data));
    copia_nodedata->num_times = original->data->num_times;
    copia_nodedata->key = original->data->key;
    copia->data = copia_nodedata;
    
    
    if (original->right== NIL){
        copia->right=NIL;
    }
    else{
        node *right_copia;
        right_copia = malloc(sizeof(node)); //Reservem espai pel node copia
        copia->right = right_copia;
        copyTreeRecursive(original->right, copia->right); //Seguim pel node->right de cada arbre
    }
}

/**
 * Donats dos arbres, copia el contingut del primer sobre el segon
 */

void copyTree(rb_tree *orgTree, rb_tree *copia){
    init_tree(copia);
    if(orgTree != NULL){
        node *x;
        x = malloc(sizeof(node));
        copia->root = x; //Creem una copia del node arrel
        copyTreeRecursive(orgTree->root, copia->root); //Copiem recursivament tots els nodes
        
    }
}

void updateTreeRecursive(node *copia, node *original) {
    
    
    if (copia->left!= NIL){
        updateTreeRecursive(copia->left, original->left);
    }
    
    //Actualitzem unicament el nombre d'aparicions
    original->data->num_times += copia->data->num_times;
    
    
    if (copia->right!= NIL){
        updateTreeRecursive(copia->right, original->right);
    }
}

/**
 * Donats dos arbres, augmenta el nombre d'aparicions de paraules del primer, sumant-li les del segon
 */

void updateTree(rb_tree *copia, rb_tree *orgTree){
    
    if(orgTree !=NULL){
        updateTreeRecursive(copia->root, orgTree->root);
        
    }
}

/**
 *
 * Free data element. The user should adapt this function to their needs.  This
 * function is called internally by deletenode and is used to free the dynamic
 * memory that is stored inside node_data. The memory is allocated by the user
 * code, just before the node is inserted in the tree. 
 *
 */

void free_node_data(node_data *data)
{
    free(data->key);
    free(data);
}


void free_node_child(node_data *data)
{
    free(data);
}

/**
 *
 * Compares if key1 is less than key2. Should return 1 (true) if condition
 * is satisfied, 0 (false) otherwise.
 *
 */

int compare_key1_less_than_key2(char *key1, char *key2)
{
    int rc;

    rc = 0;

    /* We perform a comparison of strings ignoring case */
    
    if (strcasecmp(key1, key2) < 0)
        rc = 1;

    return rc;
}

/**
 *
 * Compares if key1 is equal to key2. Should return 1 (true) if condition
 * is satisfied, 0 (false) otherwise.
 *
 */

int compare_key1_equal_to_key2(char *key1, char *key2)
{
    int rc;

    rc = 0;

    if (strcasecmp(key1, key2) == 0)
        rc = 1;

    return rc;
}

/**
 *
 *  Function used to print the number of times a word appears. Do not call directly. 
 *
 */

void dump_numtimes_words_recursive(node *x)
{
    if (x->right != NIL)
        dump_numtimes_words_recursive(x->right); 

    if (x->left != NIL)
        dump_numtimes_words_recursive(x->left); 

    printf("%s %d\n", x->data->key, x->data->num_times);
}

/**
 *
 *  Print the number of times each words appears. Main function. 
 *
 */

void dump_numtimes_words(rb_tree *tree)
{
    if (tree->root != NIL)
        dump_numtimes_words_recursive(tree->root);
}



/**
 * 
 * The red-black tree uses the sentilnel code as a leave node.
 *
 */

node sentinel = { NULL, NULL, NULL, NULL, BLACK};

/**
 *
 * Initialize the tree.
 * 
 */

void init_tree(rb_tree *tree)
{
    tree->root = NIL;
}

/**
 *
 *  Find node containing the specified key. Returns the node.
 *  Returns NULL if not found.
 *
 */

node_data *find_node(rb_tree *tree, char *key) {

    node *current = tree->root;
    while (current != NIL)
        if(compare_key1_equal_to_key2(key, current->data->key))
            return (current->data);
        else
            current = compare_key1_less_than_key2(key, current->data->key) ?
                current->left : current->right;

    return NULL;
}

void delete_tree_recursive_child(node *x)
{
    if (x->right != NIL)
        delete_tree_recursive_child(x->right);

    if (x->left != NIL)
        delete_tree_recursive_child(x->left);

    free_node_child(x->data);
    free(x);
}

/**
 *
 *  Delete a tree. All the nodes and all the data pointed to by
 *  the tree is deleted. 
 *
 */

void delete_tree_child(rb_tree *tree)
{
    if (tree->root != NIL)
        delete_tree_recursive_child(tree->root);
}

/** 
 *
 * NO FA FALTA MODIFICAR EL CODI QUE HI HA SOTA PER FER LES PRACTIQUES.
 *
 */

/**
 *
 *  Function used to delete a tree. Do not call directly. 
 *
 */

void delete_tree_recursive(node *x)
{
    if (x->right != NIL)
        delete_tree_recursive(x->right);

    if (x->left != NIL)
        delete_tree_recursive(x->left);

    free_node_data(x->data);
    free(x);
}

/**
 *
 *  Delete a tree. All the nodes and all the data pointed to by
 *  the tree is deleted. 
 *
 */

void delete_tree(rb_tree *tree)
{
    if (tree->root != NIL)
        delete_tree_recursive(tree->root);
}

/**
 *
 *  Rotate node x to left. Should not be called directly by the user. This
 *  function is used internally by other functions.
 *
 */

void rotate_left(rb_tree *tree, node *x) {

    node *y = x->right;

    /* establish x->right link */
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    } else {
        tree->root = y;
    }

    /* link x and y */
    y->left = x;
    if (x != NIL) x->parent = y;
}

/**
 *  
 *  Rotate node x to right. Should not be called directly by the user. This
 *  function is used internally by other functions.
 *
 */

void rotate_right(rb_tree *tree, node *x) {
    node *y = x->left;

    /* establish x->left link */
    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    } else {
        tree->root = y;
    }

    /* link x and y */
    y->right = x;
    if (x != NIL) x->parent = y;
}

/** 
 *
 * Maintain Red-Black tree balance  after inserting node x. Should not be
 * called directly by the user. This function is used internally by other
 * functions.
 *
 */

void insert_fixup(rb_tree *tree, node *x) {
    /* check Red-Black properties */
    while (x != tree->root && x->parent->color == RED) {
        /* we have a violation */
        if (x->parent == x->parent->parent->left) {
            node *y = x->parent->parent->right;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                /* uncle is BLACK */
                if (x == x->parent->right) {
                    /* make x a left child */
                    x = x->parent;
                    rotate_left(tree,x);
                }

                /* recolor and rotate */
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotate_right(tree, x->parent->parent);
            }
        } else {

            /* mirror image of above code */
            node *y = x->parent->parent->left;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                /* uncle is BLACK */
                if (x == x->parent->left) {
                    x = x->parent;
                    rotate_right(tree, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotate_left(tree,x->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

/**
 *  
 * Allocate node for data and insert in tree. This function does not perform a
 * copy of data when inserting it in the tree, it rather creates a node and
 * makes this node point to the data. Thus, the contents of data should not be
 * overwritten after calling this function.
 *
 */

void insert_node(rb_tree *tree, node_data *data) {
    node *current, *parent, *x;

    /* Find where node belongs */
    current = tree->root;
    parent = 0;
    while (current != NIL) {
        if (compare_key1_equal_to_key2(data->key, current->data->key)) {
            printf("insert_node: trying to insert but primary key is already in tree.\n");
            exit(1);
        }
        parent = current;
        current = compare_key1_less_than_key2(data->key, current->data->key) ?
            current->left : current->right;
    }

    /* setup new node */
    if ((x = malloc (sizeof(*x))) == 0) {
        printf("insufficient memory (insert_node)\n");
        exit(1);
    }

    /* Note that the data is not copied. Just the pointer
       is assigned. This means that the pointer to the 
       data should not be overwritten after calling this
       function. */

    x->data = data;

    /* Copy remaining data */
    x->parent = parent;
    x->left = NIL;
    x->right = NIL;
    x->color = RED;

    /* Insert node in tree */
    if(parent) {
        if(compare_key1_less_than_key2(data->key, parent->data->key))
            parent->left = x;
        else
            parent->right = x;
    } else {
        tree->root = x;
    }

    insert_fixup(tree, x);
}



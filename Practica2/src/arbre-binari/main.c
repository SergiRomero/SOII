/**
 *
 * Main file 
 * 
 * This file is an example that uses the red-black-tree.c functions.
 *
 * Lluis Garrido, July 2019.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "red-black-tree.h"

#define MAXVALUE 10
#define MAXCHAR 100

/**
 *
 *  Main function. This function is an example that shows
 *  how the binary tree works.
 *
 */

int main(int argc, char **argv)
{
  int ct;
  
  FILE *fp;
  char word[MAXCHAR];
  
  rb_tree *tree;
  node_data *n_data;

  if (argc != 2)
  {
    printf("Usage: %s maxnum\n", argv[0]);
    exit(1);
  }
  
  fp = fopen("words", "r");

  //maxnum = atoi(argv[1]);

  printf("Test with red-black-tree\n");

  /* Random seed */
  srand(time(NULL));

  /* Allocate memory for tree */
  tree = (rb_tree *) malloc(sizeof(rb_tree));

  /* Initialize the tree */
  printf("Initialize tree\n");
  init_tree(tree);

  char *a;
  
  printf("Read Doc\n");
  while (fgets(word, MAXCHAR, fp)) { //POR CADA PALABRA DE DICCIONARI

    /* Search if the key is in the tree */
    n_data = find_node(tree, word); 

    if (n_data != NULL) {

      /* If the key is in the tree increment 'num' */
      n_data->num_times++;
    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */
      
      a = malloc(sizeof(char)*(strlen(word)));
    
      strncpy(a, word, strlen(word) - 1);
      strcat(a, "\0");
      
      n_data = malloc(sizeof(node_data));
      
      /* This is the key by which the node is indexed in the tree */
      n_data->key = a;
      
      /* This is additional information that is stored in the tree */
      n_data->num_times = 1;

      /* We insert the node in the tree */
      insert_node(tree, n_data);
    }
    //char *a;
  }
  
  /* We now dump the information of the tree to screen */

  
  fclose(fp);
  
  ct = 0;

  fp = fopen("words", "r");
  while (fgets(word, MAXCHAR, fp))
  {
    n_data = find_node(tree, word);
    
    if (n_data) { 
      printf("La paraula %s apareix %d cops a l'arbre.\n", word, n_data->num_times);
      ct += n_data->num_times;
    }
  }

  printf("Nombre total que vegades que s'ha accedit a l'arbre: %d\n", ct);
  fclose(fp);
  /* Delete the tree */
  delete_tree(tree);
  free(tree);

  return 0;
}


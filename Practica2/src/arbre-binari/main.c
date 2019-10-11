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
#include <ctype.h>  

#include "red-black-tree.h"

#define MAXVALUE 10
#define MAXCHAR 100

/**
 *
 *  Main function. This function is an example that shows
 *  how the binary tree works.
 *
 */

int process_line(char *line, rb_tree *tree)
{
    
    node_data *n_data;
    int i, j, is_word, len_line, ct;
    char paraula[MAXCHAR];
    
    ct = 0;
    i = 0;

    len_line = strlen(line);

    /* Search for the beginning of a candidate word */

    while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

    /* This is the main loop that extracts all the words */

    while (i < len_line)
    {
        j = 0;
        is_word = 1;

        /* Extract the candidate word including digits if they are present */

        do {

            if (isalpha(line[i])) // '
                paraula[j] = line[i];
            else 
                is_word = 0;

            j++; i++;

            /* Check if we arrive to an end of word: space or punctuation character */

        } while ((i < len_line) && (!isspace(line[i])) && (!ispunct(line[i])));

        /* If word insert in list */

        if (is_word) {

            /* Put a '\0' (end-of-word) at the end of the string*/
            paraula[j] = 0;

            /* Print found word */
            n_data = find_node(tree, paraula);
    
            if (n_data) {
                n_data->num_times++;
                printf("La paraula %s apareix %d cops a l'arbre.\n", paraula, n_data->num_times);
                ct += 1;
            }
        }

        /* Search for the beginning of a candidate word */

        while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i])))) i++; 

    } /* while (i < len_line) */
    
    return ct;
}

int main(int argc, char **argv)
{
  int ct;
  
  FILE *fp;
  char word[MAXCHAR];
  
  rb_tree *tree;
  node_data *n_data;

  if (argc != 2)
  {
    printf("Usage: %s file\n", argv[0]);
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
  
  printf("Read DICTIONARY\n");
  while (fgets(word, MAXCHAR, fp)) { //POR CADA PALABRA DE DICCIONARI

    /* Search if the key is in the tree */
    n_data = find_node(tree, word); 

    if (n_data != NULL) {
      //YA ESTA AÑADIDO
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
      n_data->num_times = 0;

      /* We insert the node in the tree */
      insert_node(tree, n_data);
    }
    //char *a;
  }
  
  /* We now dump the information of the tree to screen */

  ct = 0;
  fclose(fp);
    char line[MAXCHAR];
    
    if (argc != 2) {
        printf("%s <file>\n", argv[0]);
        exit(1);
    }
    
    fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file\n");
        exit(1);
    }
    
    while (fgets(line, MAXCHAR, fp))
        ct += process_line(line, tree);
    
    fclose(fp);
  
  
  /*
  fp = fopen("words", "r");
  while (fgets(word, MAXCHAR, fp))
  {
      a = malloc(sizeof(char)*(strlen(word)));
      strncpy(a, word, strlen(word) - 1);
      n_data = find_node(tree, a);
    
    if (n_data) { 
      printf("La paraula %s apareix %d cops a l'arbre.\n", a, n_data->num_times);
      ct += n_data->num_times;
    }
  }
  fclose(fp);*/
  /* Delete the tree */
  
  printf("Nombre total que vegades que s'ha accedit a l'arbre: %d\n", ct);
  delete_tree(tree);
  free(tree);

  return 0;
}



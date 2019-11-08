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

/*Function that returns 1 if the character is a simbol*/
int issimbol(char caracter){
    return ((caracter=='#') || (caracter=='$') || (caracter=='/') || (caracter=='&') ||(caracter == '\\') || (caracter=='+') || (caracter=='^')||(caracter=='*')||(caracter=='%') || (caracter == '=') || (caracter=='@') || (caracter == '_') || (caracter == '~') || (caracter == '|') || (caracter == '`'));
}

int process_line(char *line, rb_tree *tree)
{

    node_data *n_data;
    int i, j, is_word, len_line, ct;
    char paraula[MAXCHAR];

    ct = 0;
    i = 0;

    len_line = strlen(line);

    /* Search for the beginning of a candidate word */
    // El caracter ASCII 39 es l'apòstrof

    while ((i < len_line) && (isspace(line[i]) || (ispunct(line[i]) && (line[i] != 39) && !(issimbol(line[i]))))) i++;

    /* This is the main loop that extracts all the words */

    while (i < len_line)
    {
        j = 0;
        is_word = 1;

        /* Extract the candidate word including digits if they are present */

        do {

            if (isalpha(line[i]) || (line[i] == 39))
                paraula[j] = line[i];
            else
                is_word = 0;

            j++; i++;

            /* Check if we arrive to an end of word: space or punctuation character */

        } while ((i < len_line) && (!isspace(line[i])) && (!ispunct(line[i]) || (line[i] == 39) || (issimbol(line[i]))));

        /* If word insert in list */

        if (is_word) {

            /* Put a '\0' (end-of-word) at the end of the string*/
            paraula[j] = 0;

            /* Print found word */
            n_data = find_node(tree, paraula);

            if (n_data) {
                n_data->num_times++;
                //printf("La paraula %s apareix %d cops a l'arbre.\n", paraula, n_data->num_times);
                ct += 1;
            }
        }

        /* Search for the beginning of a candidate word */

        while ((i < len_line) && (isspace(line[i]) ||((ispunct(line[i])) && (line[i] != 39) && !(issimbol(line[i]))))) i++;

    } /* while (i < len_line) */

    return ct;
}

/**
 *
 *  Main function. This function is an example that shows
 *  how the binary tree works.
 *
 */

int main(int argc, char **argv)
{
  int ct;

  FILE *fp, *fp2;
  int i, numFiles = 0;
  char word[MAXCHAR];

  rb_tree *tree;
  node_data *n_data;

  if (argc != 2)
  {
    printf("Usage: %s file\n", argv[0]);
    exit(1);
  }

  fp = fopen("words", "r");

  if (!fp) {
    printf("Could not open dicctionary\n");
    exit(1);
  }

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
      if (strlen(word) != 1){
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
    }
  }

  /* We now dump the information of the tree to screen */

    ct = 0;
    fclose(fp);

    char line[MAXCHAR];

    fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file1\n");
        exit(1);
    }

    if(fgets(word, MAXCHAR, fp)){
        word[strlen(word)-1]= 0;
        numFiles = atoi(word);
    }

    for(i=0; i < numFiles; i++){
        fgets(word, MAXCHAR, fp);
        word[strlen(word)-1]= 0;
        fp2=fopen(word, "r");

        if (!fp2) {
            printf("Could not open file2\n");
            exit(1);

        }
    while (fgets(line, MAXCHAR, fp2)){
        ct += process_line(line, tree);
    }
    fclose(fp2);
    }

    fclose(fp);

    inOrder(tree->root);
  
  /* Delete the tree */

  //printf("Nombre total que vegades que s'ha accedit a l'arbre: %d\n", ct);
  delete_tree(tree);
  free(tree);

  return 0;
}

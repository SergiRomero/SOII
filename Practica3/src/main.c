/**
 *
 * Main file
 *
 * This file is an example that uses the red-black-tree.c functions.
 *
 * Lluis Garrido, July 2019.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "red-black-tree.h"

#define MAXVALUE 10
#define MAXCHAR 100
#define MAGIC_NUMBER 0x01234567


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

rb_tree *fillDictionary(char *diccionari, char *baseDades)
{
    int ct;
    rb_tree *tree;
    FILE *fp, *fp2;
    int i, numFiles = 0;
    char word[MAXCHAR];

    node_data *n_data;
    
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);

    fp = fopen(diccionari, "r");

    if (!fp) {
        printf("Could not open dicctionary\n");
        exit(1);
    }

    /* Random seed */
    srand(time(NULL));

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
                
                //printf("NEW WORD\n");
                a = malloc(sizeof(char)*(strlen(word)));

                strncpy(a, word, strlen(word));
                a[strlen(word)-1]=0;

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

    fp = fopen(baseDades, "r");
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

    //inOrder(tree->root);
  

  return tree;
  
}


/*
 * --------------------------------------------------------------------------------
 *                          MENU
 *-------------------------------------------------------------------------------- 
 */

int menu() 
{
    char str[5];
    int opcio;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre\n");
    printf(" 2 - Emmagatzemar arbre a disc\n");
    printf(" 3 - Llegir arbre de disc\n");
    printf(" 4 - Consultar informacio de l'arbre\n");
    printf(" 5 - Sortir\n\n");
    printf("   Escull opcio: ");

    fgets(str, 5, stdin);
    opcio = atoi(str); 

    return opcio;
}


int main(int argc, char **argv)
{
    char str1[MAXCHAR], str2[MAXCHAR];
    int opcio;
    rb_tree *tree;
    
    tree = NULL;
    
    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    do {
        opcio = menu();
        printf("\n\n");

       /* Feu servir aquest codi com a pantilla */

        switch (opcio) {
            case 1:
                
                if(tree) {
                    printf("Free tree memory\n");
                    delete_tree(tree);
                    free(tree);
                }
                
                printf("Fitxer de diccionari de paraules: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                printf("Fitxer de base de dades: ");
                fgets(str2, MAXCHAR, stdin);
                str2[strlen(str2)-1]=0;

                tree = fillDictionary(str1, str2);

                break;

            case 2:
                if(!tree) {
                    printf("No hi ha cap arbrea memòria\n");
                    break;
                }
                
                printf("Nom de fitxer en que es desara l'arbre: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */

                break;

            case 3:
                if(tree) {
                    printf("Free tree memory\n");
                    delete_tree(tree);
                    free(tree);
                }
                
                
                printf("Nom del fitxer que conte l'arbre: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */

                break;

            case 4:
                
                inOrder(tree->root);
                
                printf("Paraula a buscar o polsa enter per saber la paraula que apareix mes vegades: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */

                break;

            case 5:
                if(tree) {
                    printf("Free tree memory\n");
                    delete_tree(tree);
                    free(tree);
                }
                break;

            default:
                printf("Opcio no valida\n");

        } /* switch */
    }
    while (opcio != 5);
    
    return 0;
}






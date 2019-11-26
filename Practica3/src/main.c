/**
 *
 * Main file
 *
 * This file is an example that uses the red-black-tree.c functions.
 *
 * Lluis Garrido, July 2019.
 * 
 * Sergi Romero i Juan Cano
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
 *  
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

    for(i=0; i < numFiles; i++) {
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
 * ---------------------------------------------------------
 * 
 *              LOAD TREE
 * 
 * ---------------------------------------------------------
 */

/*
 * Funcio que carrega la informacio del diccionari dins de l'arbre. Retorna l'arbre omplert.
 * 
 */

rb_tree *loadTree(char *diccionari) {
    
    rb_tree *tree;
    FILE *fp;
    
    node_data *n_data;
    
    int i, magic, numNodes, lenWord, numInstances;
    char* word;
    
    
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    init_tree(tree);
    
    fp = fopen(diccionari, "r");

    if (!fp) {
        printf("Could not open dicctionary\n");
        exit(1);
    }
    
    //COMPROVACIO MAGIC NUMBER
    fread(&magic, sizeof(int), 1, fp);
    if (magic != MAGIC_NUMBER){
        printf("Wrong magic number\n");
        exit(1);
    }
    
    //COMPROVACIO NUM NODES
    fread(&numNodes, sizeof(int), 1, fp);
    if (numNodes <= 0){
        printf("Empty tree\n");
        exit(1);
    }
    
    
    for(i=0; i < numNodes; i++) {
        
        fread(&lenWord, sizeof(int), 1, fp);
        if (lenWord <= 0){
            printf("Wrong len word\n");
            exit(1);
        }
        
        word = malloc(sizeof(char)*(lenWord + 1)); //Reservem espai pel \0
        fread(word, sizeof(char)*lenWord, 1, fp);
        word[lenWord] = 0;
        
        fread(&numInstances, sizeof(int), 1, fp);
        if (numInstances < 0){
            printf("Negative word apearence\n");
            free(word); //Zona de memoria inconexa amb l'arbre
            exit(1);
        }
        
        n_data = malloc(sizeof(node_data));

        n_data->key = word;
        n_data->num_times = numInstances;
        insert_node(tree, n_data);
    }
    
    fclose(fp);
    
    return tree;
}

/*
 * -----------------------------------------------------
 * 
 *                   SAVE TREE
 * 
 * -----------------------------------------------------
 */

/*
 * Funcio que desa l'arbre.
 * 
 */

//---------------------- COUNT NODES ----------------------

int countNodesRecursive(node *x)
{
    int i;

    i = 0;

    //Recorregut en profunditat
    if (x->right != NIL)
        i += countNodesRecursive(x->right);

    if (x->left != NIL)
        i += countNodesRecursive(x->left);
    
    //Comptem el node
    i += 1;

    return i;
}

int countNumNodes(rb_tree *tree)
{
    int i;

    i = countNodesRecursive(tree->root);

    return i;

}

//---------------------- SAVE DATA ----------------------

void saveNodeData(node *x, FILE *fp){
    
    //DEFINE HOW WE SAVE DATA
    int i;
    
    i = strlen(x->data->key);
    fwrite(&i, sizeof(int), 1, fp);

    fwrite(x->data->key, sizeof(char), i, fp);

    i = x->data->num_times;
    fwrite(&i, sizeof(int), 1, fp);    
}

void saveNodeDataRecursive(node *x, FILE *fp) {

    //Recorregut en profunditat
    if (x->right != NIL)
        saveNodeDataRecursive(x->right, fp);

    if (x->left != NIL)
        saveNodeDataRecursive(x->left, fp);
    
    saveNodeData(x, fp);
}

void saveTree(rb_tree *tree, char *diccionari) {
    
    FILE *fp;
    int numNodes, magicNumber;
    
    //fwrite(&i, sizeof(int), 1, fp);
    
    fp = fopen(diccionari, "w");
    if (!fp) {
        printf("Could not open file\n");
        exit(1);
    }
    
    magicNumber = MAGIC_NUMBER;
    fwrite(&magicNumber, sizeof(int), 1, fp);
    
    numNodes = countNumNodes(tree);
    fwrite(&numNodes, sizeof(int), 1, fp);
    
    saveNodeDataRecursive(tree->root, fp);
    
    fclose(fp);
}

/*
 * --------------------------------------------------------------------------------
 *
 *                      CONSULTES ARBRE
 *
 * --------------------------------------------------------------------------------
 */

int buscarParaula(rb_tree *tree, char *paraula)
{
    node_data* n_data;
    n_data = find_node(tree, paraula);
    return n_data->num_times;
}



node_data *searchMaxNumDataRecursive(node *x)
{
    node_data *nodeMax;
    node_data *nodeTemp;

    nodeMax = x->data;
    if (x->right != NIL){
        nodeTemp =  searchMaxNumDataRecursive(x->right);
        if (nodeTemp->num_times > nodeMax->num_times){
            nodeMax = nodeTemp;
        }
    }
    if (x->left != NIL){
        nodeTemp =  searchMaxNumDataRecursive(x->left);
        if (nodeTemp->num_times > nodeMax->num_times){
            nodeMax = nodeTemp;
        }
    }

    return nodeMax;
}


node_data *searchMaxNumData(rb_tree * tree)
{
    return searchMaxNumDataRecursive(tree->root);
}



/*
 * --------------------------------------------------------------------------------
 *                          MENU
 * -------------------------------------------------------------------------------- 
 */

/*
 * Funcio que ens mostra el menu amb les diferentes a escollir (crear, emmagatzemar, llegir, consultar informacio de l'arbre, i sortir)
 * 
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

/*
 * Funcio que ens mostra el submenu, corresponent a l'opcio de consultar informacio de l'arbre. Aquest submenu te les opcions de veure el nombre que apareix una paraula entrada i comprovar la paurala que apareix mes cops.
 * 
 */
int subMenu() 
{
    char str[5];
    int opcio;

    printf("\n\nQuè vols consultar?\n\n");
    printf(" 1 - Comptador d'una paraula\n");
    printf(" 2 - Paraula que apareix més\n");
    printf(" 3 - Sortir\n\n");
    printf("   Escull opcio: ");

    fgets(str, 5, stdin);
    opcio = atoi(str); 

    return opcio;
}

/*
 * --------------------------------------------------------------------------------
 *                          MAIN
 * -------------------------------------------------------------------------------- 
 */

int main(int argc, char **argv)
{
    char str1[MAXCHAR], str2[MAXCHAR];
    int opcio, opcio2;
    int numTimes;
    rb_tree *tree;
    node_data *maxTimes;
    
    tree = NULL;
    
    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    do {
        opcio = menu();
        printf("\n\n");

       /* Codi sitwch-case agafat com a plantilla */

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
                    printf("No hi ha cap arbre a memòria\n");
                    break;
                }
                
                printf("Nom de fitxer en que es desara l'arbre: ");
                fgets(str1, MAXCHAR, stdin);
                str1[strlen(str1)-1]=0;

                saveTree(tree, str1); 

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

                tree = loadTree(str1);

                break;

            case 4:
                if(!tree) {
                    printf("No hi ha cap arbrea memòria\n");
                    break;
                }

                do {
                    opcio2 = subMenu();
                    printf("\n\n");

                    switch(opcio2){
                        case 1:
                            printf("Paraula a buscar: ");
                            fgets(str1, MAXCHAR, stdin);
                            str1[strlen(str1)-1]=0;

                            numTimes = buscarParaula(tree, str1);
                            
                            printf("La paraula %s apareix %i cops\n", str1, numTimes);
                            break;

                        case 2:
                            maxTimes = searchMaxNumData(tree);
                            printf("La paraula %s apareix %i cops\n", maxTimes->key, maxTimes->num_times);
                            break;

                    } /* switch */
                } while (opcio2 != 3);
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






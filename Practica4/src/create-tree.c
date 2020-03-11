#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 

#include <sys/wait.h>
#include <unistd.h>

#include <semaphore.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

#include "red-black-tree.h"
#include "tree-to-mmap.h"
#include "dbfnames-mmap.h"

#define MAXCHAR 100

typedef struct shared_mem{
  int nFile;
  sem_t sem_read;
  sem_t sem_write;
} shared_mem;

 shared_mem *estructura; //Definim una variable global de tipus struct que conte els 2 semafors i el numero de fitxer pel qual anem

/**
 *
 *  Given a file, insert the words it contains into a tree.  We assume that
 *  each line contains a word and that words are not repeated. 
 *
 */

void index_dictionary_words(rb_tree *tree, FILE *fp)
{
  char *s, word[MAXCHAR];

  node_data *n_data;

  while (fgets(word, MAXCHAR, fp))
  {
    // Remove '\n' from string
    word[strlen(word)-1] = 0;

    // In the last line of the dictionary
    // an empty string is read
    if (strlen(word) > 0) {
      // Copy static string to dynamic string
      s = malloc(strlen(word)+1);
      strcpy(s, word);

      // Insert into tree
      n_data = malloc(sizeof(node_data));

      n_data->key = s;
      n_data->num_times = 0;
      insert_node(tree, n_data);
    }
  }
}

/**
 *
 *  Given a line with words, extract them and transform them to
 *  lowercase. We search each word in the tree. If it is found,
 *  we increase the associated counter. Otherwise nothing is done
 *  for that word.
 *
 */

void index_words_line(rb_tree *tree, char *line)
{
  node_data *n_data;

  int i, j, is_word, len_line;
  char paraula[MAXCHAR];

  i = 0;

  len_line = strlen(line);

  /* Search for the beginning of a candidate word */

  while ((i < len_line) && (isspace(line[i]) || ((ispunct(line[i])) && (line[i] != '#')))) i++; 

  /* This is the main loop that extracts all the words */

  while (i < len_line)
  {
    j = 0;
    is_word = 1;

    /* Extract the candidate word including digits if they are present */

    do {

      if ((isalpha(line[i])) || (line[i] == '\''))
        paraula[j] = line[i];
      else 
        is_word = 0;

      j++; i++;

      /* Check if we arrive to an end of word: space or punctuation character */

    } while ((i < len_line) && (!isspace(line[i])) && (!(ispunct(line[i]) && (line[i]!='\'') && (line[i]!='#'))));

    /* If word insert in list */

    if (is_word) {

      /* Put a '\0' (end-of-word) at the end of the string*/
      paraula[j] = 0;

      /* Search for the word in the tree */
      n_data = find_node(tree, paraula);
      
      sem_wait(&estructura->sem_write); //Abans d'escriure ens esperem fins que ho puguem fer

      if (n_data != NULL)
        n_data->num_times++;
      
      sem_post(&estructura->sem_write); //Un cop hem escrit, tornem a obrir la escriptura
      
    }

    /* Search for the beginning of a candidate word */

    while ((i < len_line) && (isspace(line[i]) || ((ispunct(line[i])) && (line[i] != '#')))) i++; 

  } /* while (i < len_line) */
}

/**
 *
 *  Given a file, this funcion reads the lines it contains and uses function
 *  index_words_line to index all dictionary words it contains.
 *
 */

void process_file(rb_tree *tree, char *fname)
{
  FILE *fp;
  char line[MAXCHAR];

  fp = fopen(fname, "r");
  if (!fp) {
    printf("Could not open %s\n", fname);
    exit(1);
  }

  while (fgets(line, MAXCHAR, fp))
    index_words_line(tree, line);

  fclose(fp);
}

/**
 *
 *  Construct the tree given a dictionary file and a 
 *  database with files to process.
 *
 */


rb_tree *create_tree(char *fname_dict, char *fname_db)
{
  FILE *fp_dict, *fp_db;

  rb_tree *tree;
  int i = 0;
  int ret;
  char *line;
  char *mapped_tree;
  char *mapped_db;
  int numProcessadors = get_nprocs();

  fp_dict = fopen(fname_dict, "r");
  if (!fp_dict) {
    printf("Could not open dictionary file %s\n", fname_dict);
    return NULL;
  }

  fp_db = fopen(fname_db, "r");
  if (!fp_db) {
    printf("Could not open dabase file %s\n", fname_db);
    return NULL;
  }

  /* Allocate memory for tree */
  tree = (rb_tree *) malloc(sizeof(rb_tree));

  /* Initialize the tree */
  init_tree(tree);

  /* Index dictionary words */
  index_dictionary_words(tree, fp_dict);

  mapped_tree = serialize_node_data_to_mmap(tree); //serialitzem les dades de l'arbre

  mapped_db = dbfnames_to_mmap(fp_db); //Mapem la llista de fitxers a memoria

  /* Read database files */
  
  estructura = mmap(NULL, sizeof(shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //Mapem la estructura dels semafors a memoria
  if (estructura == MAP_FAILED) {
    printf("Map failed. Errno = %d\n", errno);
    exit(1);
  }
  estructura->nFile = 0;
  
  //Inicialitzacio dels semafors
  sem_init(&estructura->sem_write, 1, 1); /* Indiquem que esta compartida entre procesos */
  sem_init(&estructura->sem_read, 1, 1); /* Indiquem que esta compartida entre procesos */

  /* Close files */
  fclose(fp_dict);
  fclose(fp_db);
    
  //CHILD BORNS
  while(i < numProcessadors){ //Creem tants fills com processadors te el sistema
   
    ret = fork(); //Creem un fill
    i++;
    
    if(ret == 0){ //Si es un fill, no ha de crear fills, per tant surt del while
      break;
    }  
    
  }    
  

  if (ret == 0) {  // FILL

	printf("Child on the road\n");
    do {
        
        sem_wait(&estructura->sem_read); //Esperem fins que podem llegir dades
        line = get_dbfname_from_mmap(mapped_db, estructura->nFile);
        //printf("Processing %s   %i  %i\n", line, estructura->nFile, getpid());
        estructura->nFile++;
        sem_post(&estructura->sem_read); //Un cop hem llegit, tornem a habilitar la lectura
        
        if (line == NULL){
            break;
        }
        
		/* Process file */
		process_file(tree, line);
        
		i ++;
		
	} while (line != NULL);
    
    delete_tree_child(tree); //delete tree child metode que nomes esborra la key sense free de data
    free(tree); //alliberem l'arbre
	exit(1);
    
  } // END FILL
  
  //FATHER
  for(i = 0; i < numProcessadors; i++){ //El pare fa un wait per cada fill, ja que ha desperar a que tots els fills acabin
    wait(NULL);
  }
  
  printf("LEAVING FATHER\n");
  
  //Destruccio dels semafors
  sem_destroy(&estructura->sem_read);
  sem_destroy(&estructura->sem_write);

  dbfnames_munmmap(mapped_db); //deserialitzem el mapatge del fitxer de la base de dades
  deserialize_node_data_from_mmap(tree, mapped_tree);//deserialitzem les dades de l'arbre
  munmap(estructura, sizeof(shared_mem)); //deserialitzem el struct
  

  /* Return created tree */
  return tree;
}

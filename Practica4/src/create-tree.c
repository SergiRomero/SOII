#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 

#include <sys/wait.h>
#include <unistd.h>

#include "red-black-tree.h"
#include "tree-to-mmap.h"
#include "dbfnames-mmap.h"

#define MAXCHAR 100

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

      if (n_data != NULL)
        n_data->num_times++;
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
  int i;
  int ret;
  char *line;
  char *mapped_tree;
  char *mapped_db;

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

  mapped_tree = serialize_node_data_to_mmap(tree);

  mapped_db = dbfnames_to_mmap(fp_db); //Mapamos la litsa de ficheros a memoria

  /* Read database files */

  //CHILD BORNS

  ret = fork();

  if (ret == 0) {  // fill

	printf("Child on the road\n");
    i = 0;
	line = get_dbfname_from_mmap(mapped_db, i);
	do {
    
		printf("Processing %s\n", line);

		/* Process file */
		process_file(tree, line);
    
		i ++;
		line = get_dbfname_from_mmap(mapped_db, i);
	} while (line != NULL);

	exit(1);
  } else { // pare
    printf("FATHER CODE\n");
	wait(NULL);
	printf("FINISHED WAIT\n");
  }
  printf("LEAVING FATHER\n");

  dbfnames_munmmap(mapped_db);
  deserialize_node_data_from_mmap(tree, mapped_tree);
  
  /* Close files */
  fclose(fp_dict);
  fclose(fp_db);

  /* Return created tree */
  return tree;
}

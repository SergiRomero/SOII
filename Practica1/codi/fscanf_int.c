#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  FILE *fp;
  int i;

  if (argc != 3) {
    printf("%s <file> <number_ints>\n", argv[0]);
    exit(1);
  }

  fp = fopen(argv[1], "w");
  if (!fp) {
    printf("Could not open file\n");
    exit(1);
  }
 
  while (fscanf(fp, "%d", &i))
    printf("%d\n", i);

  printf("Closing the file\n");
  
  fclose(fp);

  return 0;
}

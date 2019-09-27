#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int i, *a;

	a = malloc(-1); /* Malloc no valid */

	for(i = 0; i < 10; i++)
		a[i] = 0;

	return 0;
}	

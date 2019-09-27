#include <stdio.h>
#include <stdlib.h>

void funcio(int *a, int k)
{
    int i, j;

    j = 0;
    for(i = 0; i < 10; i++)
    {
        if ((i == 5) && (k == 2))
        {
            a = 0x0;
            j = 1;
        }

        a[i] = i;
    }
}

int main(int argc, char **argv)
{
    int *a;

    a = malloc(sizeof(int) * 10);

    funcio(a, 0);
    funcio(a, 1);
    funcio(a, 2);

    return 0;
}	

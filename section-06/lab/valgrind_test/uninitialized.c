#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *i = malloc(sizeof(int));

    printf("%d\n", *i); /* Use of uninitialized memory */

    free(i);    
    return 0;
}
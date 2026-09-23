#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *i = malloc(sizeof(int));
    *i = 10;

    free(i);

    printf("%d\n", *i); /* Use after free */
    return 0;
}
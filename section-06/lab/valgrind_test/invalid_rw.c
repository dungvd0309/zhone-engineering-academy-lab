#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *str = malloc(10);
    strcpy(str, "0123456789");

    free(str);
    return 0;
}
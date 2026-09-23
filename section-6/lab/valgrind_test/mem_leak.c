#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *str = malloc(20);
    strcpy(str, "Hello, World!");
    printf("%s\n", str);

    // free(str);
    return 0;
}
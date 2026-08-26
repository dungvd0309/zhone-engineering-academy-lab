#include <stdio.h>
#include <stdlib.h>

int global_initialized_var = 42; /* Data - Initialized global variable */

int global_uninitialized_var; /* BSS - Uninitialized global variable */

void some_function(void) { /* Text - Function code */
    printf("Inside some_function\n");
}

int main(int argc, char *argv[]) {
    int stack_var = 10;                 /* Stack - Local variable */
    static int static_var = 100;        /* Data - Initialized static variable */
    int *heap_var = malloc(sizeof(int)); /* Heap - Dynamically allocated variable */
    *heap_var = 5;

    printf("Text (code) segment:\n");
    printf("  main function       : %p\n", (void *)main);
    printf("  some_function       : %p\n\n", (void *)some_function);

    printf("Data segment (initialized):\n");
    printf("  global_initialized_var : %p\n", (void *)&global_initialized_var);
    printf("  static_var (in main)   : %p\n\n", (void *)&static_var);

    printf("BSS segment (uninitialized):\n");
    printf("  global_uninitialized_var: %p\n\n", (void *)&global_uninitialized_var);

    printf("Heap segment:\n");
    printf("  heap_var (malloc'd) : %p\n\n", (void *)heap_var);

    printf("Stack segment:\n");
    printf("  stack_var (local)   : %p\n", (void *)&stack_var);
    printf("  argv pointer        : %p\n\n", (void *)argv);

    printf("Environment variables:\n");
    extern char **environ;
    printf("  environ[0]          : %p\n\n", (void *)environ[0]);

    free(heap_var);
    return 0;
}
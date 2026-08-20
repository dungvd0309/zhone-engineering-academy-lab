#include <stdio.h>

/* Define a function pointer type for operations */
typedef int (*OperationCallback)(int, int);

/* Function to perform a calculation using a callback */
void calculator(int a, int b, OperationCallback callback)
{
    printf("%d\n", callback(a, b));
}

/* Operation enum */
typedef enum {
    ADD, // 0
    SUBTRACT, // 1
    MULTIPLY, // 2
    DIVIDE // 3
} Operation;

/* Function implementations */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }

/* Dispatch table for operations */
OperationCallback operations[] = { add, subtract, multiply, divide };

int main(void)
{
    int a = 10, b = 2;
    calculator(a, b, operations[ADD]); // 12
    calculator(a, b, operations[SUBTRACT]); // 8
    calculator(a, b, operations[MULTIPLY]); // 20
    calculator(a, b, operations[DIVIDE]); // 5
    return 0;
}


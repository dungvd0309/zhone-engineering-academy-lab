#include <stdio.h>
#include <pthread.h>

long counter = 0; /* global variable */

void* increment(void* arg) /* thread function */
{
    for (int i = 0; i < 1000000; i++) 
    {
        counter++;   
    }
    return NULL;
}

int main() 
{
    pthread_t t1, t2;

    /* Create two threads that do the incrementing */
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    /* Wait for both threads to complete */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Result: %ld (expected: 2000000)\n", counter);
    return 0;
}
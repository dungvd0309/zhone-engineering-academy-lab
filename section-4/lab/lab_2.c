#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
long counter = 0; /* global variable */

void* increment(void* arg) /* thread function */
{
    for (int i = 0; i < 1000000; i++) 
    {
        pthread_mutex_lock(&mutex);
        counter++;   
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() 
{
    pthread_mutex_init(&mutex, NULL);
    pthread_t t1, t2;

    /* Create two threads that do the incrementing */
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    /* Wait for both threads to complete */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Result: %ld (expected: 2000000)\n", counter);

    pthread_mutex_destroy(&mutex);
    return 0;
}
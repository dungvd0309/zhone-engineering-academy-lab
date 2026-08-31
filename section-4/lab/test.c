#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> 

pthread_mutex_t mutex;
int count = 0;

void* producer(void* arg) 
{
    char* threadName = "Producer";
    // test print
    printf("%s: Starting\n", threadName);
}

void* consumer(void* arg) {
    /* Consumer thread name */
    char* threadName = malloc(20 * sizeof(char));
    sprintf(threadName, "Consumer %s", (char*)arg);

    printf("%s: Starting\n", threadName);
    
    free(threadName);
    return NULL;
}

int main() {
    char* threadName = "Main Thread";

    pthread_mutex_init(&mutex, NULL);

    pthread_t prod_t, cons1_t, cons2_t;
    pthread_create(&prod_t, NULL, producer, NULL);
    pthread_create(&cons1_t, NULL, consumer, "1");
    pthread_create(&cons2_t, NULL, consumer, "2");

    pthread_join(prod_t, NULL);
    pthread_join(cons1_t, NULL);
    pthread_join(cons2_t, NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}
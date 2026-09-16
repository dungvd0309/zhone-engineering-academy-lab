#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
long counter = 0;

void* producer(void* arg) 
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        counter += 1;   
        printf("[Producer] produced 1, counter = %ld\n", counter);
        pthread_cond_signal(&cond); 
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* consumer(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (counter < 4) /* wait for the producer to produce */
            pthread_cond_wait(&cond, &mutex);
        
        counter -= 4; /* consume items */
        printf("[Consumer] consumed 4, counter = %ld\n", counter);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() 
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, consumer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
long counter = 0; /* global variable */

void* thread_a(void* arg) 
{
    while (1)
    {
        // pthread_mutex_lock(&mutex2);
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);
        counter++;
        printf("[Thread A] counter = %ld\n", counter);
        pthread_mutex_unlock(&mutex2);
        pthread_mutex_unlock(&mutex1);
        usleep(100000); 
    }
    return NULL;
}

void* thread_b(void* arg) 
{
    while (1)
    {
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);
        counter++;
        printf("[Thread B] counter = %ld\n", counter);
        pthread_mutex_unlock(&mutex2);
        pthread_mutex_unlock(&mutex1);
        usleep(100000);
    }
    return NULL;
}

void* thread_c(void* arg) /* deadlock timeout check */
{
    while (1)
    {
        long old_counter = counter;
        sleep(1);
        if (old_counter == counter)
        {
            printf("[Thread C] Deadlock detected!\n");
            exit(-1);
        }
    }
    return NULL;
}

int main() 
{
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, thread_a, NULL);
    pthread_create(&t2, NULL, thread_b, NULL);
    pthread_create(&t3, NULL, thread_c, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    return 0;
}
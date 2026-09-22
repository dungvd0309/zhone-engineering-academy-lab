#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2
#define OPS_PER_THREAD 200000

typedef struct {
    int *data;
    int size;
    int capacity;
} DynArray;

DynArray arr;

void arr_push(DynArray *a, int value) {
    if (a->size == a->capacity) {
        a->capacity *= 2;
        a->data = realloc(a->data, a->capacity * sizeof(int));
    }
    a->data[a->size] = value;
    a->size++;
}

void *worker(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        arr_push(&arr, (int)id);
    }
    return NULL;
}

int main(void) {
    arr.capacity = 4;
    arr.size = 0;
    arr.data = malloc(arr.capacity * sizeof(int));

    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, worker, (void *)i);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("Final size: %d (expected %d)\n", arr.size, NUM_THREADS * OPS_PER_THREAD);

    long sum = 0;
    for (int i = 0; i < arr.size; i++)
        sum += arr.data[i];
    printf("Sum: %ld\n", sum);

    free(arr.data);
    return 0;
}
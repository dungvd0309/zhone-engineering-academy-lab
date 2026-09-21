#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define PROGRAM 1 /* 0: Non-reentrant, 1: Reentrant */

#if PROGRAM == 0

char *get_message(const char *name) /* Non-reentrant function */
{
    static char buffer[100]; /* Non-reentrant since using a static buffer */
    snprintf(buffer, sizeof(buffer), "Hi, I'm %s", name);
    sleep(1);

    return buffer;
}

void *thread_func(void *arg)
{
    const char *name = (const char *)arg;
    char *msg = get_message(name);
    printf("[%s thread] %s\n", name, msg);

    return NULL;
}

#endif

#if PROGRAM == 1

char *get_message(const char *name, char *buffer, size_t buffer_size) /* Reentrant function */
{
    snprintf(buffer, buffer_size, "Hi, I'm %s", name);
    sleep(1);

    return buffer;
}

void *thread_func(void *arg)
{
    const char *name = (const char *)arg;

    char buffer[100]; /* The thread provides buffer */
    char *msg = get_message(name, buffer, sizeof(buffer));
    printf("[%s thread] %s\n", name, msg);

    return NULL;
}

#endif

int main(void)
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_func, "Alice");
    pthread_create(&t2, NULL, thread_func, "Bob");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
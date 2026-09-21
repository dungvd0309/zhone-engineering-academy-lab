#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>

struct shared_data {
    sem_t sem_full;
    sem_t sem_empty;
    int value;
};

const int MAX_SIZE = 5;
const char SHM_NAME[] = "/my_shm";
const size_t SHM_LENGTH = sizeof(struct shared_data);

int fd;     /* File descriptor for shared memory */
struct shared_data* addr;  /* Pointer to the shared memory */

void handler(int sig) {
    printf("Signal %d received. Cleaning up resource...\n", sig);

    /* Clean up semaphores */
    sem_destroy(&addr->sem_full);
    sem_destroy(&addr->sem_empty);

    /* Clean up shared memory */
    munmap(addr, SHM_LENGTH);
    shm_unlink(SHM_NAME);
    _exit(0);
}

void init_signal()
{
    struct sigaction sa;
    
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler; 
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
}

void print_value(const char* name, const int value)
{
    printf("[%s] value = %i\n", name, value);
}

void producer(const char* name)
{   
    /* Creation of shared memory */
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SHM_LENGTH);
    addr = mmap(NULL, SHM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return;
    }

    /* Initialize binary semaphores */
    sem_init(&addr->sem_empty, 1, 1);
    sem_init(&addr->sem_full, 1, 0);
    addr->value = 0;

    while(1) 
    {
        sem_wait(&addr->sem_empty);

        /* Produce items */
        for(int i = 0; i < MAX_SIZE; i++) 
        {
            addr->value++;
            print_value(name, addr->value);
            usleep(500000);
        }

        sem_post(&addr->sem_full);
    }

    /* Removal of shared memory */
    munmap(addr, SHM_LENGTH);
    shm_unlink(SHM_NAME);
}

void consumer(const char* name)
{   
    /* Creation of shared memory */
    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    ftruncate(fd, SHM_LENGTH);
    addr = mmap(NULL, SHM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return;
    }

    while(1) 
    {
        sem_wait(&addr->sem_full);
        
        /* Consume items */
        for(int i = 0; i < MAX_SIZE; i++) 
        {
            addr->value--;
            print_value(name, addr->value);
            usleep(500000);
        }

        sem_post(&addr->sem_empty);
    }

    /* Removal of shared memory */
    munmap(addr, SHM_LENGTH);
    shm_unlink(SHM_NAME);
}

int main() 
{
    init_signal();

    switch(fork()) 
    {
        case -1:
            perror("fork");
            return -1;
        case 0: // Child process
            usleep(500000); // Ensure producer starts first
            consumer("Consumer");
            break;
        default: // Parent process 
            producer("Producer");
            break;
    }
    return 0;
}
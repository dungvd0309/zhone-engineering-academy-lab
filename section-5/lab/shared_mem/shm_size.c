#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
    int shm_size = 10;

    /* Create a new shared memory segment */
    int shmid = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        perror("shmget");
        return -1;
    }

    /* Attach the segment to virtual memory */
    void *baseaddr = shmat(shmid, NULL, 0);
    if (baseaddr == (void *)-1) 
    {
        perror("shmat");
        return -1;
    }
    
    /* Mark for removal when no processes attached */
    shmctl(shmid, IPC_RMID, NULL); 

    printf("Shared memory attached at: %p\n", baseaddr);
    printf("Process ID: %d\n", getpid());

    getchar(); 
    shmdt(baseaddr); /* Detach the segment */
    return 0;
}
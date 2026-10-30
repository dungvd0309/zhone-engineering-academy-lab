/* writer */
#include <stdio.h>
#include <sys/shm.h>

int main()
{
    /* Create a new shared memory segment */
    int shmid = shmget(123, 4096, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        perror("shmget");
        return -1;
    }

    /* Attach the segment to virtual memory */
    long *baseaddr = shmat(shmid, NULL, 0);
    if (baseaddr == (void *)-1) 
    {
        perror("shmat");
        return -1;
    }
    
    /* Pointers for referencing the shared memory */
    long *p = &baseaddr[1]; 
    long *target = &baseaddr[50];
    
    /* Store a value in the shared memory */
    *target = 123456789;

    /* Store a pointer to target in the shared memory */
    *p = (long)target; /* Convert address to long*/

    /* Access the shared memory */
    printf("Memory addresses:\n");
    printf("- baseaddr of shared memory: %p\n", baseaddr);
    printf("- p in shared memory: %p\n", p);
    printf("- target in shared memory: %p\n", target);

    printf("Values:\n");
    printf("- target value: %ld\n", *target);
    printf("- p value: %p\n", (long *)*p);
    printf("- p dereferenced value: %ld\n", *(long *)(*p));
    
    /* Detach the segment */
    shmdt(baseaddr); 
    return 0;
}
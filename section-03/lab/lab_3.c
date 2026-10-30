#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();

    if (pid == 0) {
        printf("[Child] PID: %d, PPID: %d\n", getpid(), getppid());
        sleep(100);
        exit(EXIT_SUCCESS);
    }
    printf("[Parent] PID: %d\n", getpid());
    wait(NULL);
    exit(EXIT_SUCCESS);
}
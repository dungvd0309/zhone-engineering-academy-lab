#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        usleep(100000);

        printf("Child: Hello!\n");
        fflush(stdout);

        exit(0);
    }

    usleep(100000);

    printf("Parent: kill()\n");
    fflush(stdout);
    kill(pid, SIGKILL);

    waitpid(pid, NULL, 0);
}
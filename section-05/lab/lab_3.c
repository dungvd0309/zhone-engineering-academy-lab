#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig)
{
    printf("SIGINT received.\n");
    _exit(0);
}

void sigalrm_handler(int sig)
{
    printf("SIGALRM received.\n");
}

int main()
{
    struct sigaction sa;

    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);

    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sa, NULL);

    printf("Set an alarm for 5 seconds.\n");
    alarm(5); 
    
    printf("Waiting for signals...\n");
    pause();
    
    return 0;
}
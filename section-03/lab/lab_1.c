#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sleep_with_print(char *name, int running_time, int print_interval) 
{
    for (int i = 0; i < running_time; i += print_interval) 
    {
        printf("[%s] PID: %d, PPID: %d, Time: %d\n", 
            name, getpid(), getppid(), i);
        sleep(print_interval);
    }
}

int main()
{   
    int print_interval = 2; /* Print interval in seconds */

    pid_t child_pid = fork();
    if(child_pid == -1) 
    {
        perror("fork error");
        return 1;
    } 
    else if(child_pid == 0) /* Child process */
    { 
        pid_t grandchild_pid = fork();
        if(grandchild_pid == -1) 
        {
            perror("fork error");
            return 1;
        } 
        else if(grandchild_pid == 0) /* Grandchild process */
        { 
            sleep_with_print("Grandchild", 30, print_interval);
            execlp("echo", "echo", "Bye!!! Grandchild process using execlp to echo", NULL);
            
            exit(EXIT_SUCCESS);
        } 
        else /* Child process */
        { 
            sleep_with_print("Child process", 5, print_interval);
            printf("Child process - PID %d exiting\n", getpid());

            exit(EXIT_SUCCESS);
        }
    } 
    else /* Parent process */
    { 
        sleep_with_print("Parent process", 60, print_interval);
        waitpid(child_pid, NULL, 0); 
        printf("parent waited!\n");
        sleep_with_print("Parent process", 5, print_interval);

        printf("Parent process - PID %d exiting\n", getpid());

        exit(EXIT_SUCCESS);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

char user_name[50];
char friend_name[50];

char fifo_send[100]; 
char fifo_recv[100];
char recv_buffer[254];

pthread_t send_thread;
pthread_t recv_thread;

void* send_thread_func(void* arg) 
{
    char send_buffer[254];
    while (1) 
    {
        /* Read input from the user */
        if (fgets(send_buffer, sizeof(send_buffer), stdin) != NULL) 
        {
            /* Open the FIFO for writing */
            int fd = open(fifo_send, O_WRONLY);
            if (fd == -1) 
            {
                printf("%s is not online.\n", friend_name);
                continue;
            }

            /* Write data to the FIFO */
            write(fd, send_buffer, strlen(send_buffer));
            printf("You -> %s: %s", friend_name, send_buffer);

            /* Close the FIFO after writing */
            close(fd);
        }
    }
    return NULL;
}

void* recv_thread_func(void* arg) 
{
    while (1) 
    {
        /* Open the FIFO for reading */
        int fd = open(fifo_recv, O_RDONLY);
        if (fd == -1) 
        {
            perror("open fifo_recv");
            printf("fifo_recv got deleted by accident?\n");
            exit(-1);
        }

        /* Read data from the FIFO */
        if(read(fd, recv_buffer, sizeof(recv_buffer) - 1) > 0)
        {
            printf("%s -> You: %s\n", friend_name, recv_buffer);
        }

        /* Close the FIFO after sender is done */
        close(fd);
    }
    return NULL;
}

void cleanup_fifo(int sig) 
{
    unlink(fifo_recv); 
    _exit(0);
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <your_name> <friend_name>\n", argv[0]);
        return -1;
    }

    /* Store the user and friend names */
    snprintf(user_name, sizeof(user_name), "%s", argv[1]);
    snprintf(friend_name, sizeof(friend_name), "%s", argv[2]);

    /* Name the FIFOs based on names*/
    snprintf(fifo_send, sizeof(fifo_send), "%s_to_%s", argv[1], argv[2]);
    snprintf(fifo_recv, sizeof(fifo_recv), "%s_to_%s", argv[2], argv[1]);

    /* Create FIFOs */
    // mkfifo(fifo_send, 0666);
    mkfifo(fifo_recv, 0666); /* The client only create the receive FIFO */

    /* Clean up the FIFO on exit */
    signal(SIGINT, cleanup_fifo);
    signal(SIGKILL, cleanup_fifo);

    /* Create threads */
    pthread_create(&send_thread, NULL, send_thread_func, NULL);
    pthread_create(&recv_thread, NULL, recv_thread_func, NULL);
     
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    exit(0);
}
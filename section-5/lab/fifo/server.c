#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

const char *FIFO_NAME = "./my_fifo";

void cleanup_fifo(int sig) 
{
    unlink(FIFO_NAME);
    exit(0);
}

int main()
{
    char buffer[254];

    /* Create a FIFO */
    mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /* Clean up the FIFO on SIGINT */
    signal(SIGINT, cleanup_fifo);

    printf("Server is up. Waiting for messages...\n");

    while (1)
    {
        /* Open the FIFO for reading */
        int fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) 
        {
            perror("open");
            exit(-1);
        }

        /* Read data from the FIFO */
        int bytes_read;
        while((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytes_read] = '\0';   // null-terminate the string
            printf("Received: %s\n", buffer);
        }

        /* Close the FIFO after sender is done */
        close(fd);
    }
     
    exit(0);
}
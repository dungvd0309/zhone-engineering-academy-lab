#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

const int MAX_BUFFER = 4096;

int   cli_port     = 0; /* 0: Kernel-assigned port */
char  serv_ip[15]    = "127.0.0.1";
int   serv_port    = 8080;

int sockfd, len;
struct sockaddr_in servaddr, cliaddr;

void sig_int(int signum)
{
    close(sockfd);
    _exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
    char send_buf[MAX_BUFFER], read_buf[MAX_BUFFER];
    
    while(fgets(send_buf, MAX_BUFFER, fp) != NULL)
    {
        write(sockfd, send_buf, strlen(send_buf));
        if(read(sockfd, read_buf, MAX_BUFFER) == 0)
        {
            printf("Server terminated prematurely\n");
            exit(-1);
        }
        printf("Server echo: %s", read_buf);
    }
}

int main(int argc, char *argv[])
{
    if(argc > 1)
        cli_port = atoi(argv[1]); /* Custom client port */
    if(argc > 2)
        strcpy(serv_ip, argv[2]); /* Custom server IP */
    if(argc > 3)
        serv_port = atoi(argv[3]); /* Custom server port */

    signal(SIGINT, sig_int);

    /* Create a socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }

    /* Optional port binding for client */
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(cli_port);
    if(cli_port != 0)
    {
        bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    }

    /* Configure the server address */
    inet_pton(AF_INET, serv_ip, 
            (void *)&servaddr.sin_addr.s_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(serv_port);

    /* Connect to the server */
    printf("Connecting to the server %s:%d\n", serv_ip, serv_port);

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) 
        != 0)
    {
        perror("connect");
        exit(-1);
    }
    
    printf("Connected to the server!\n");

    str_cli(stdin, sockfd);

    close(sockfd);

    return 0;
}
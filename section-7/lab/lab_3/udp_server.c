#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

const char  IP_ADDRESS[]    = "0.0.0.0";
const int   PORT            = 1234;
const int   MAX_BUFFER      = 4096;

void print_server_info(struct sockaddr_in *servaddr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(servaddr->sin_addr), ip, INET_ADDRSTRLEN);
    printf("Server IP: %s\n", ip);
    printf("Server Port: %d\n", ntohs(servaddr->sin_port));
}

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{   
    ssize_t n;
    socklen_t len;
    char buffer[MAX_BUFFER];
    for(;;)
    {
        len = clilen;
        memset(buffer, 0, sizeof(buffer));
        n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, pcliaddr, &len);
        printf("[%s:%d] sent %ld bytes: %s\n", 
                inet_ntoa(((struct sockaddr_in *)pcliaddr)->sin_addr), 
                ntohs(((struct sockaddr_in *)pcliaddr)->sin_port), 
                n,
                buffer
                );
        sendto(sockfd, buffer, n, 0, pcliaddr, len);
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    /* Socket init */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&servaddr, 0, sizeof(servaddr));

    /* Configure the server address */
    inet_pton(AF_INET, IP_ADDRESS, 
            (void *)&servaddr.sin_addr.s_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    /* Bind the socket to the server address */
    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) 
    {
        perror("bind");
        exit(-1);
    }

    print_server_info(&servaddr);

    memset(&cliaddr, 0, sizeof(cliaddr));
    dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}
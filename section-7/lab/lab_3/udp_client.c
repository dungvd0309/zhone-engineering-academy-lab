#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

const int MAX_BUFFER = 4096;


char  serv_ip[15]    = "127.0.0.1";
int   serv_port    = 1234;

void dg_echo(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen)
{   
    int n;
    socklen_t len;
    char buffer[MAX_BUFFER];

    while(fgets(buffer, MAX_BUFFER, fp) != NULL)
    {
        len = servlen;
        sendto(sockfd, buffer, strlen(buffer), 0, pservaddr, len);
        n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, pservaddr, &len);
        printf("Server echo: %s", buffer);
    }

}

int main(int argc, char *argv[])
{
    if(argc > 1)
        strcpy(serv_ip, argv[1]); /* Custom server IP */
    if(argc > 2)
        serv_port = atoi(argv[2]); /* Custom server port */

    int sockfd;
    struct sockaddr_in servaddr;

    /* Socket init */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&servaddr, 0, sizeof(servaddr));

    /* Configure the server address */
    inet_pton(AF_INET, serv_ip, 
            (void *)&servaddr.sin_addr.s_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(serv_port);

    dg_echo(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
}
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1234

int main()
{
    int sockfd, connfd, len;

    struct sockaddr_in servaddr, cliaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) 
    {
        perror("bind");
        exit(-1);
    }

    if(listen(sockfd, 10) != 0)
    {
        perror("listen");
        exit(-1);
    }

    printf("Server listening on port %d\n", PORT);

    len = sizeof(cliaddr);

    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);

    if(connfd == 0)
    {
        printf("Success\n");
    }
    else 
    {
        perror("accept");
        exit(-1);
    }

    close(connfd);
    close(sockfd);
    return 0;
}
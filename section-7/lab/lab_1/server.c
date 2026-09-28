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
#include <unistd.h>

const char  IP_ADDRESS[]    = "0.0.0.0";
const int   PORT            = 8080;

const int   MAX_CONNECTIONS = 5;
const int   MAX_BUFFER      = 4096;

void print_server_info(struct sockaddr_in *servaddr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(servaddr->sin_addr), ip, INET_ADDRSTRLEN);
    printf("Server IP: %s\n", ip);
    printf("Server Port: %d\n", ntohs(servaddr->sin_port));
}

void str_echo(int connfd)
{
    char buffer[MAX_BUFFER];
    ssize_t n_read;

    /* Get the client socket address struct*/
    struct sockaddr_in cliaddr;
    char cli_ip[INET_ADDRSTRLEN];
    uint16_t cli_port;
    socklen_t len = sizeof(cliaddr);
    getpeername(connfd, (struct sockaddr *)&cliaddr, &len);
    cli_port = ntohs(cliaddr.sin_port);
    inet_ntop(AF_INET, &(cliaddr.sin_addr), cli_ip, INET_ADDRSTRLEN);

    /* Server loop */
    printf("[%s:%d] Connected\n", cli_ip, cli_port);
    while(1)
    {
        /* Read data from client */
        memset(buffer, 0, sizeof(buffer));
        n_read = read(connfd, (void *)buffer, sizeof(buffer));

        if(n_read <= 0) break;

        /* Echo the client data */
        write(connfd, buffer, strlen(buffer));
        printf("[%s:%d] sent %ld bytes: %s\n", 
            cli_ip, cli_port, n_read, buffer);
    }
    
    printf("[%s:%d] Closed connection\n", cli_ip, cli_port);
    close(connfd);
}

int main()
{
    int listenfd, connfd, len;
    struct sockaddr_in servaddr, cliaddr;
    int yes=1;

    uint32_t s_addr;
    inet_pton(AF_INET, IP_ADDRESS, (void *)&s_addr);

    /* Server socket address initialization */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = s_addr;
    servaddr.sin_port = htons(PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Bind the socket to the server address */
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) 
    {
        perror("bind");
        exit(-1);
    }

    /* Listen for incoming connections */
    if(listen(listenfd, MAX_CONNECTIONS) != 0)
    {
        perror("listen");
        exit(-1);
    }

    print_server_info(&servaddr);

    /* Accept a connection from a client */
    len = sizeof(cliaddr);
    for(;;) {
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        if(connfd < 0)
        {
            perror("accept");
            exit(-1);
        }
        if ( fork() == 0 )
        {
            
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
    
    close(listenfd);
    return 0;
}
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
#include <sys/wait.h>
#include <sys/select.h>
#include <time.h>

typedef struct {
    int fd;
    struct sockaddr_in addr;
    char name[30];
    time_t last_msg_time;
} client_t;

const char  IP_ADDRESS[]    = "0.0.0.0";
const int   PORT            = 8080;

const int   MAX_CLIENTS     = 3;
const int   MAX_LISTEN      = 3;
const int   MAX_BUFFER      = 4096;
const int   MSG_COOLDOWN    = 1; /* second */

const char  SERVER_NAME[]   = "Server";

struct sockaddr_in servaddr;    /* Server address */
int listenfd;                   /* FD for server */
client_t *clients;              /* Array of clients */
int n_client = 0;               /* Number of clients */

fd_set allset, rset;
int maxfd;   /* Max FD value for select() */

void print_server_info(struct sockaddr_in *servaddr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(servaddr->sin_addr), ip, INET_ADDRSTRLEN);
    printf("Server IP: %s\n", ip);
    printf("Server Port: %d\n", ntohs(servaddr->sin_port));
}

void addr_to_str(struct sockaddr_in *addr, char *buf, size_t size)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
    snprintf(buf, size, "%s:%d", ip, ntohs(addr->sin_port));
}

void send_msg(client_t client, char *buf)
{
    write(client.fd, buf, strlen(buf));
}

void broadcast_msg(client_t *clients, char *buf, int exceptfd)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(clients[i].fd != -1 && clients[i].fd != exceptfd)
        {
            write(clients[i].fd, buf, strlen(buf));
        }
    }
}

void sig_int(int sig)
{
    exit(0);
}

void exit_handler()
{
    /* Broadcast to all clients on exit */
    printf("\nServer shutting down...\n");
    broadcast_msg(clients, "Server shutting down.\n", -1);
    close(listenfd);
    free(clients);
}

int main()
{
    char recv_buf[MAX_BUFFER];  
    char send_buf[MAX_BUFFER]; 

    /* Allocate memory for the client array */
    clients = (client_t *)malloc(sizeof(client_t) * MAX_CLIENTS);
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].fd = -1;  

    signal(SIGINT, sig_int);
    atexit(exit_handler);
    
    /* Socket init */
    int yes=1;
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Server address init*/
    memset(&servaddr, 0, sizeof(servaddr));
    inet_pton(AF_INET, IP_ADDRESS, 
            (void *)&servaddr.sin_addr.s_addr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    /* Bind the socket to the server address */
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) 
    {
        perror("bind");
        exit(-1);
    }

    /* Listen for incoming connections */
    if(listen(listenfd, MAX_LISTEN) != 0)
    {
        perror("listen");
        exit(-1);
    }

    /* FD set init */
    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    print_server_info(&servaddr);
    
    /* Main loop */
    for(;;) {

        /* Select each loop iteration */
        rset = allset;
        select(maxfd + 1, &rset, NULL, NULL, NULL);

        /* Accept a new connection */
        if(FD_ISSET(listenfd, &rset)) 
        {   
            int is_added = 0;

            /* Create a temporary client*/
            client_t client;
            socklen_t len = sizeof(client.addr);
            client.fd = accept(listenfd, (struct sockaddr *)&client.addr, &len);
            addr_to_str(&client.addr, client.name, sizeof(client.name));
            
            /* Store the client in the client list if it's not full*/
            for(int i = 0; i < MAX_CLIENTS; i++)
            {
                if(clients[i].fd < 0) 
                {
                    clients[i] = client;
                    is_added = 1;
                    break;
                }
            }

            if(!is_added) 
            {
                printf("[%s] Max clients reached. Connection from %s refused.\n", SERVER_NAME, client.name);
                send_msg(client, "Server is full. Try again later.\n");
                close(client.fd);
                continue;
            }

            /* Add the new client's FD to allset */
            FD_SET(client.fd, &allset); 
            if(client.fd > maxfd) maxfd = client.fd;

            /* Broadcast to everyone */
            sprintf(send_buf, "[%s] %s connected.\n", SERVER_NAME, client.name);
            printf("%s", send_buf);
            broadcast_msg(clients, send_buf, -1);
        }
        
        /* Check and handle each client FDs */
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            client_t* pclient = &clients[i];

            /* Ignore unset FD */
            if (pclient->fd < 0)
                continue;


            if(FD_ISSET(pclient->fd, &rset))
            {
                int n = read(pclient->fd, recv_buf, MAX_BUFFER);
                
                /* Client disconnected */
                if(n <= 0) 
                {
                    close(pclient->fd);
                    FD_CLR(pclient->fd, &allset);
                    pclient->fd = -1;

                    /* Broadcast to everyone*/
                    sprintf(send_buf, "[%s] %s has disconnected.\n", SERVER_NAME, pclient->name);
                    printf("%s", send_buf);
                    broadcast_msg(clients, send_buf, -1);
                    continue;
                }

                /* Data received from client */
                time_t now = time(NULL);
                if(now - pclient->last_msg_time >= MSG_COOLDOWN)
                {  
                    recv_buf[n] = '\0';
                    recv_buf[strcspn(recv_buf, "\r\n")] = '\0';

                    /* Change name command */
                    if (strncmp(recv_buf, "/name ", 6) == 0)
                    {
                        char *new_name = recv_buf + 6;
                        if (strlen(new_name) > 0 && strlen(new_name) < sizeof(pclient->name))
                        {
                            char old_name[sizeof(pclient->name)];
                            strcpy(old_name, pclient->name);
                            strcpy(pclient->name, new_name);
                            sprintf(send_buf, "[%s] %s changed name to %s.\n", SERVER_NAME, old_name, pclient->name);
                            printf("%s", send_buf);
                            broadcast_msg(clients, send_buf, -1);
                        }
                        else
                        {
                            sprintf(send_buf, "[%s] Name must be between 1 and %lu characters.\n", SERVER_NAME, sizeof(pclient->name) - 1);
                            send_msg(*pclient, send_buf);
                        }
                        continue;
                    }

                    /* Echo back to sender */
                    sprintf(send_buf, "[You] %s\n", recv_buf);
                    send_msg(*pclient, send_buf);

                    /* Broadcast to others */
                    sprintf(send_buf, "[%s] %s\n", pclient->name, recv_buf);
                    broadcast_msg(clients, send_buf, pclient->fd);
            
                    printf("%s", send_buf);

                    pclient->last_msg_time = now;
                }
                
            }
        }
    }
    
    return 0;
}
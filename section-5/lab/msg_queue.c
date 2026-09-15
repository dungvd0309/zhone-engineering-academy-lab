#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>

typedef struct MyMsg {
    long mtype;     
    char mtext[1024];   
} MyMsg;

int main() 
{
    int msgid = msgget(123456, IPC_CREAT | 0666);

    if(msgid == -1) 
    {
        perror("msgget");
        return -1;
    }

    switch (fork()) 
    {
        case -1:
            perror("fork");
            return -1;

        case 0: /* Child process recv */
            const int BUFFER_SIZE = 1024;
            MyMsg recv_msg;

            /* Receive a message from the message queue */
            msgrcv(msgid, &recv_msg, BUFFER_SIZE, 0, 0);

            /* Extract the long value from the message text */
            long recv_key;
            memcpy(&recv_key, recv_msg.mtext, sizeof(long));
            
            printf("Received msg: %li \n", recv_key);

            break;

        default: /* Parent process send */
            /* Initialize a message*/
            MyMsg send_msg;
            send_msg.mtype = 1;

            /* Send a long value by copying it to the message text */
            long super_important_key = 1234567890123456789;
            memcpy(send_msg.mtext, &super_important_key, sizeof(long));

            /* Send message to message queue*/
            msgsnd(msgid, &send_msg, sizeof(send_msg.mtext), 0);
  
            break;
    }

    return 0;
}
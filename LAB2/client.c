#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#define MAX 256

// structure for message queue
struct msg {
    long mtype;
    char mtext[MAX];
} send, recieve;
  
int main()
{
    int nc, nr, i;
    int pid = getpid();

    key_t key, key1;
    int msgid, msgid1;
  

    key = ftok("HASH", 96);
    key1 = ftok("HASH", 69);
  

    msgid = msgget(key, 0666 | IPC_CREAT);
    msgid1 = msgget(key1, 0666 | IPC_CREAT);
    send.mtype = 2;
    
    sprintf(send.mtext, "%d", pid);
    msgsnd(msgid1, &send, sizeof(send), 0);

    msgrcv(msgid, &recieve, sizeof(recieve), 1, 0);  
    nc = atoi(recieve.mtext);

    nr = 0;

    while(1){
        sleep(1);
        // RECIEVING END
        printf("\n\t\t\tROUND : %d\n\n", nr++);
        msgrcv(msgid, &recieve, sizeof(recieve), 1, 0);

        
        printf("QUESTION : %s \n", recieve.mtext);

        for(i = 0; i < nc; ++i){
            printf("\nClient : %d, write your ans.\n", i);
            fgets(send.mtext, MAX, stdin);
            send.mtext[strlen(send.mtext) - 1] = 0;
            msgsnd(msgid1, &send, sizeof(send), 0);
            printf("\n");
        }

    }
  
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>

#define MAX 256
#define MAX_SCORE 50
#define inf 10000
  
// structure for message queue
struct msg {
    long mtype;
    char mtext[MAX];
} send, recieve;

int H(char *S){
    int score = 0;
    for(char *x = S; *x != '\0'; ++x){
        score += *x;
    }
    score %= strlen(S);
    return score;
}
  
int main()
{
    int nc, H_S, H_r, nr, i; //nc -> number of clients
    int score[MAX] = {0};
    int mn, mni[MAX], nm, tmp, pid;

    key_t key, key1;
    int msgid, msgid1;
  

    key = ftok("HASH", 96);
    key1 = ftok("HASH", 69);


    msgid = msgget(key, 0666 | IPC_CREAT);
    msgid1 = msgget(key1, 0666 | IPC_CREAT);
    send.mtype = 1;

    // take the clients pid
    msgrcv(msgid1, &recieve, sizeof(recieve), 2, 0);
    pid = atoi(recieve.mtext);

    // read and send number of clients...
    printf("Enter number of clients you wanted => ");
    fgets(send.mtext, MAX, stdin);
    send.mtext[strlen(send.mtext) - 1] = 0;
    nc = atoi(send.mtext);
    msgsnd(msgid, &send, sizeof(send), 0);
  

    nr = 0;
    // ROUNDS STARTS
    int ok = 1;
    while(ok){
        // SENDING END
        printf("\n\t\t\tROUND : %d\n\n", nr++);
        printf("Write a Question, to ask to all clients.\n");
        fgets(send.mtext,MAX,stdin);
        printf("Question recieved by us : %s\n", send.mtext);
        send.mtext[strlen(send.mtext) - 1] = 0;

        H_S = H(send.mtext);

        msgsnd(msgid, &send, sizeof(send), 0);

        // RECIEVING END
        mn = inf, nm = 0;
        for(i = 0; i < nc; ++i){
            msgrcv(msgid1, &recieve, sizeof(recieve), 2, 0);
            H_r = H(recieve.mtext);
            printf("Response recieved by client %d : %s\n\n", i, recieve.mtext);
            tmp = abs(H_S - H_r);
            if(tmp < mn) mn = tmp, nm = 0;
            if(tmp == mn) mni[nm++] = i;
        }

        printf("Winners of This Round : ");
        for(i = 0; i < nm; ++i){
            printf("%d ", i);
            score[mni[i]] += 5;
        }

        printf("\n\nCurrent scores : \n");
        for(i = 0; i < nc; ++i){
            printf("Client %d : %d\n", i, score[i]);
            if(score[i] >= MAX_SCORE){ ok = 0; }
        }
        if(!ok){
            kill(pid, SIGQUIT);
        }
    }
    
    printf("\nTHE WINNERs Are : \n");
    for(i = 0; i < nc; ++i){
        if(score[i] >= MAX_SCORE){
            printf("%d ", i);
        }
    }
    printf("\n\n");
  
    msgctl(msgid, IPC_RMID, NULL);
    msgctl(msgid1, IPC_RMID, NULL);
    
    return 0;
}
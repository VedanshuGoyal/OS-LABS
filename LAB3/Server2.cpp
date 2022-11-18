#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

void error(const char *msg){
	perror(msg);
	exit(1);
}

#define MAX 255
#define LISTENQ 8

void go(char tt){
	//ftok to generate unique key
    key_t key = ftok("HASH", 69);
  
    // shmget returns an identifier in shmid
    int shmid = shmget(key, MAX, 0666|IPC_CREAT);
  
    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid, (void*)0, 0);
  
    str[0] = tt;
      
    //detach from shared memory 
    shmdt(str);
}


int sockfd, newsockfd;

void handler(int num){
	close(sockfd);
	close(newsockfd);
	go('0');
	exit(1);
}

int calculate(char a[]){
	string s = "";
	int i = 0, x, y;
	char oper;
	
	while(a[i] == ' ') i++;

	while(a[i] - '0' > -1 && a[i] - '9' < 1){
		s += a[i++];
	}
	x = stoi(s);

	while(a[i] == ' ') i++;
	oper = a[i++];

	while(a[i] == ' ') i++;

	s = "";
	while(a[i] - '0' > -1 && a[i] - '9' < 1){
		s += a[i++];
	}
	y = stoi(s);

	switch(oper){
		case '+':
			return x + y;

		case '-':
			return x - y;

		case '*':
			return x * y;

		case '/':
			return x / y;

		default:
			raise(SIGINT);
			return 0;
	}
}

int main(int argc, char* argv[]){
	if(argc != 2){
		fprintf(stderr, "usage %s <server-port>\n", argv[0]);
		exit(1);
	}
	go('0');
	
	signal(SIGINT, handler);
	char 		buffer[MAX];
	struct 		sockaddr_in serv_addr, cli_addr;
	int 		portno, n, i;
	socklen_t	clilen;
	pid_t		childpid;

	portno = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("Error opening socket.");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("Binding Failed.");
	}
	if(listen(sockfd, LISTENQ) < 0) error("Listen Error.");


	while(1){
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0) error("Error on Accept.");

		printf("Conntected with a client.\n");
		if((childpid = fork()) == 0){
			//child's process

			close(sockfd);

			while(1){
				bzero(buffer, MAX);
				n = read(newsockfd, buffer, MAX);
				if(n < 0) error("Error on reading");

				printf("Client : %s\n", buffer);
				if(strlen(buffer) < 3) break;
				string ans = to_string(calculate(buffer));

				bzero(buffer, MAX);
				for(i = 0; i < ans.size(); ++i){
					buffer[i] = ans[i];
				}
				buffer[i] = 0;
				printf("Sending Reply : %s\n\n", buffer);

				n = write(newsockfd, buffer, strlen(buffer));
				if(n < 0) error("Error on writing");
			}
			
			printf("Disconnected with a client.\n");
			close(newsockfd);
			exit(0);
		}

		close(newsockfd);
	}

	close(sockfd);
	return 0;

}
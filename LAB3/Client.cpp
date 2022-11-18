#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

void error(const char *msg){
	perror(msg);
	exit(1);
}

#define MAX 255

int ck = 0;

void go(){
	// ftok to generate unique key
    key_t key = ftok("HASH", 69);
  
    // shmget returns an identifier in shmid
    int shmid = shmget(key, MAX, 0666|IPC_CREAT);
  
    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid, (void*)0, 0);

	ck = (str[0] == '1');

    //detach from shared memory 
    shmdt(str);
}

int sockfd;
void handler(int num){
	close(sockfd);
	exit(1);
}

int main(int argc, char* argv[]){
	if(argc < 3){
		fprintf(stderr, "usage %s <server-ip-addr> <server-port>\n", argv[0]);
		exit(1);
	}

	signal(SIGINT, handler);

	int sockfd, portno, n;
	char buffer[MAX];

	struct sockaddr_in serv_addr;
	struct hostent *server;

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("Error opening socket.");
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "Error, no such host");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	
	go();
	if(ck == 1){
		printf("Line Busy!, Try after sometime...\n");
		close(sockfd);
		return 0;
	}

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("Connection Failed");
	}
	printf("Connected to server.\n");

	printf("Welecome to MathWorld.\nPlease enter the operation to perform.\nFormat : a+b | a-b | a/b | a*b\n\n");


	while(1){

		bzero(buffer, MAX);
		printf("Enter the message to the server: ");
		fgets(buffer, MAX, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		if(n < 0) error("Error on writing");
		
		bzero(buffer, MAX);
		n = read(sockfd, buffer, MAX);
		if(n < 0) error("Error on reading");

		printf("Server: %s\n", buffer);
	}
	
	close(sockfd);
	return 0;

}
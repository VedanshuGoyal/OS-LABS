#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

#define SERVERPORT		6996
#define BUFSIZE			255

typedef struct sockaddr_in	SA_IN;
typedef struct sockaddr 	SA;


#define IOSIGNAL 	SIGUSR1
#define TERMINATE 	SIGUSR2

#define READY 		2
#define IOOVER 		4

void check(int exp, const char *msg){
	if(exp < 0){
		perror(msg);
		exit(1);
	}
}

sigset_t *sett;
int sig;

bool cker;

void handle_notify(int num){
	// do nothing just to resume from pause
}

void handle_suspend(int num){
	if(cker) return;
	cker = 1;
	sigwait(sett, &sig);
	cker = 0;
}

void add(string &s, string t){ // add with delimiter
	s += t;
	s.push_back('$');
}


int connect_server(){
	int server_socket;

	SA_IN serv_addr;
	struct hostent *server;

	check(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
		"Error opening socket.");

	server = gethostbyname("localhost"); //127.0.0.1

	if(server == NULL){
		fprintf(stderr, "Error, no such host");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(SERVERPORT);

	check(connect(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), 
		"Connection Failed");


	return server_socket;
}

char buffer[BUFSIZE];
int server_socket;
int s_pid; // scheduler pid

void send_msg(string &msg){
	strcpy(buffer, msg.c_str());
	buffer[msg.size()] = 0;
	check(write(server_socket, buffer, strlen(buffer)), 
			"Error on sending");
}

void rcv_msg(){
	bzero(buffer, BUFSIZE);
	check(read(server_socket, buffer, BUFSIZE), 
		"Error on receiving");
}



int main(int argc, char* argv[]){
	srand(time(NULL));
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sett = &set;

	// handling signals
	signal(SIGUSR1, handle_notify);
	signal(SIGUSR2, handle_suspend);

	int i, pid, round, priority;
	double prob, sleeptime;
	string msg;

	pid = getpid();
	round = atoi(argv[1]);
	priority = atoi(argv[2]);
	prob = atof(argv[3]);
	sleeptime = atof(argv[4]);

	cout << round << ' ' << priority << ' ' << prob << ' ' << sleeptime << endl;

	server_socket = connect_server();
	printf("Connected to server.\n");

	// send information regarding state $ pid $ priority
	msg = "";
	add(msg, to_string(READY));
	add(msg, to_string(pid));
	add(msg, to_string(priority));
	send_msg(msg);
	rcv_msg();
	s_pid = atoi(buffer);

	handle_suspend(1);	// waiting for the NOTIFY signal

	cout << "STARTED" << endl;
	int tp = 10;
	for(i = 0; i < round; ++i){
		cout << i << endl;
		// if(--tp == 0){
		// 	tp = 10;
		// 	cout << endl;
		// }

		double r = rand() * 1.0/RAND_MAX;

		if(r > prob){
			// I/O START
			kill(s_pid, IOSIGNAL);
			handle_suspend(1);

			cout << "DOING IO" << endl;
			sleep(1);			
			cout << "FINISH IO" << endl;

			// I/O OVER
			msg = "";
			add(msg, to_string(IOOVER));
			send_msg(msg);
			handle_suspend(1); //pause 
		}
	}

	// SENDING TERMINATING SIGNAL
	kill(s_pid, TERMINATE);
	close(server_socket);

	cout << "FINISHED" << endl;
	getchar();
}
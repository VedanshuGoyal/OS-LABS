#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

using namespace std;

#define SERVERPORT		6996
#define BUFSIZE			255
#define SERVER_BACKLOG	10
#define TIME_QUANTUM	100000000

typedef struct sockaddr_in	SA_IN;
typedef struct sockaddr 	SA;

#define NOTIFY 	SIGUSR1
#define SUSPEND SIGUSR2

#define READY 		2
#define IOOVER 		4

bool ok; // ok = 1 PRR else RR

queue<array<int, 2>> Ready_Queue[11]; // {process_id, priority}
array<int, 2> cur; // cur process
int NoT = 0; // # of Terminated Process
int Tot = 4; // # of Total Process
bool intercepted; // is Current Process is in I/O or Terminate

int server_socket;
int pid;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	c = PTHREAD_COND_INITIALIZER;

void check(int exp, const char *msg){
	if(exp < 0){
		perror(msg);
		exit(1);
	}
}

void made_socket(){ //made a server connection
	SA_IN 		server_addr;
	socklen_t	addr_size;

	check(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
			"Error opening socket.");
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVERPORT);


	check(bind(server_socket, (SA*) &server_addr, sizeof(server_addr)), 
			"Binding Failed.");
	check(listen(server_socket, SERVER_BACKLOG), 
			"Listen Error.");
}

void insert(array<int, 2> &x){
	if(ok){
		Ready_Queue[x[1]].emplace(x);
	}else{
		Ready_Queue[0].emplace(x);
	}
}

bool findcur(){
	if(ok){
		int i = 10;
		while(i && Ready_Queue[i].empty()) --i;
		cout << i << endl;
		if(i == 0) return 0;
		cur = Ready_Queue[i].front();
		Ready_Queue[i].pop();
	}else{
		if(Ready_Queue[0].empty()) return 0;
		cur = Ready_Queue[0].front();
		Ready_Queue[0].pop();
	}
	// for(int &x : ioq){
	// 	if(x == cur[0]) return 0;
	// }

	return 1;
}

void* handle_connection(void *pclient){
	int client_socket = *(int *)pclient;
	array<int, 2> ck;
	int i = 0, state = 0, j;
	string tmp;
	char buffer[BUFSIZE];

	// cout << "CLIENT AddING " << endl;
	
	bzero(buffer, BUFSIZE);
	check(read(client_socket, buffer, BUFSIZE), 
		"Error on receiving");

	// printf("Client : %s\n", buffer);


	for(j = 0; j < 3; ++j){
		tmp = "";
		while(buffer[i] != '$'){
			tmp += buffer[i++];
		}
		if(j) ck[j-1] = stoi(tmp);
		else state = stoi(tmp);
		i++;
	}
	cout << state << ' ' << ck[0] << ' ' << ck[1] << endl;


	bzero(buffer, BUFSIZE);
	sprintf(buffer, "%d", pid);
	check(write(client_socket, buffer, strlen(buffer))
			,"Error on writing");

	// sleep(.05);

	pthread_mutex_lock(&m);
	insert(ck);
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);

	// IO TACKLING
	while(1){
		bzero(buffer, BUFSIZE);
		check(read(client_socket, buffer, BUFSIZE),
			"Error on receiving");
		if(buffer[0] < '0' || buffer[0] > '9') break;
		
		i = 0; tmp = "";
		while(buffer[i] != '$'){
			// cout << buffer[i] << endl;
			tmp += buffer[i++];
		}

		state = stoi(tmp);

		pthread_mutex_lock(&m);
		insert(ck);
		pthread_cond_signal(&c);
		pthread_mutex_unlock(&m);

		cout << ck[0] << " completes I/O" << endl;
	}

	close(client_socket);

	return NULL;

}


void* ADDER(void* arg){
	int 		client_socket, childpid;
	SA_IN 		client_addr;
	socklen_t	addr_size;

	while(1){
		// fetching client
		printf("Waiting for Connections...\n");

		addr_size = sizeof(SA_IN);
		check(client_socket = accept(server_socket, (SA*) &client_addr, 
			&addr_size), "Accept Failed.");
		
		printf("Connected!\n");

		pthread_t t;
		int *pclient = (int *)malloc(sizeof(int));
		*pclient = client_socket;
		pthread_create(&t, NULL, handle_connection, (void *)pclient);

		// handle_connection(client_socket);
	}

	return NULL;
}

void wait(){
	for(int tmp = 1e6; tmp; --tmp);
}


// I/O queue
queue<int> ioq;
pthread_cond_t 	c1 = PTHREAD_COND_INITIALIZER;

void* IOManager(void *arg){
	while(1){
		pthread_mutex_lock(&m);
		while(ioq.empty()){
			pthread_cond_wait(&c1, &m);
		}

		kill(ioq.front(), NOTIFY);
		ioq.pop();

		pthread_mutex_unlock(&m);
	}
}

// I/O & TERMINATE MANAGER
void* REMOVER(void* arg){
	sigset_t *set = (sigset_t *)arg;
	int sig;

	pthread_t t;
	pthread_create(&t, NULL, IOManager, NULL);

	while(1){
		sigwait(set, &sig);
		intercepted = 1;

		pthread_mutex_lock(&m);

		if(sig == 10){
			// I/O SIGNAL
			ioq.emplace(cur[0]);
			pthread_cond_signal(&c1);
			cout << cur[0] << " requests I/O" << endl;
		}
		else if(sig == 12){
			// TERMINATE SIGNAL
			++NoT;
			cout << cur[0] << " completes Process" << endl; 
		}
		pthread_mutex_unlock(&m);
		

		if(NoT == Tot) break;
	}

	return NULL;
}

int main(int argc, char* argv[]){

	pid = getpid();

	// BLOCKING IOSIGNAL AND TERMINATE SIGNAL FOR ALL
	// AND GIVING ACCESS TO THIS SIGNAL TO REMOVER THREAD

	sigset_t set;
	pthread_t adder, remover;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	made_socket();
	
	pthread_create(&remover, NULL, REMOVER, &set);
	pthread_create(&adder, NULL, ADDER, NULL);

	ok = (argv[1] == "PRR");

	// MAIN THREAD
	int i;
	while(NoT != Tot){
		pthread_mutex_lock(&m);
		while(findcur() == 0){
			pthread_cond_wait(&c, &m);
		}
		intercepted = 0;
		pthread_mutex_unlock(&m);

		kill(cur[0], NOTIFY);
		cout << cur[0] << " is running" << endl;

		for(i = 0; i < TIME_QUANTUM; ++i){
			// process is running
			if(intercepted) break;
		}

		pthread_mutex_lock(&m);
		if(!intercepted){
			// cout << "FINISH FROM ROUND ROBIN" << endl;

			// if it is ended by our round robin {noP by i/o or termination}
			// then we have to insert again it to the ready queue
			kill(cur[0], SUSPEND);
		}
		pthread_mutex_unlock(&m);

		if(!intercepted){
			insert(cur);
		};

	}
	pthread_join(remover, NULL);
}
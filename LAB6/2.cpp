#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define NOP	5 // # of PHILOSOPHERS

#define THINKING	0
#define HUNGRY		1
#define EATING		2

#define LEFT(i)		i ? i-1 : NOP-1
#define RIGHT(i)	(i != NOP-1) ? i+1 : 0

sem_t F[NOP]; // FORKS, F[0] is left fork of Phil[0]
sem_t ck, ck1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

bool DLH[NOP]; // FOR DEADLOCK HANDLING

bool G[2*NOP][2*NOP]; // adjacency matrix of the graph

void think(){
	sleep(1 + rand()%5); // random second between 1 to 5
}

void eating(){
	sleep(1 + rand()%3); // random second between 1 to 3 
}

void take_fork(int i, int j){ // resource, process
	pthread_mutex_lock(&m);
	G[j+=NOP][i] = 1; // Adding edge from j process to i resource
	pthread_mutex_unlock(&m);
	
	sem_wait(&F[i]);
	
	pthread_mutex_lock(&m);
	G[j][i] = 0;
	G[i][j] = 1;
	pthread_mutex_unlock(&m);
}

void put_fork(int i, int j){

	sem_post(&F[i]);

	pthread_mutex_lock(&m);
	G[i][j+NOP] = 0;
	pthread_mutex_unlock(&m);
}

void* philospher(void *arg){
	int i = *(int *)arg;
	srand(time(NULL) + i);
	
	while(1){
		printf("%d STARTS THINKING\n", i);
		think();

		printf("%d BECOMES HUNGRY\n", i);
		take_fork(i, i);
		printf("%d TAKE THE LEFT FORK\n", i);
		take_fork(RIGHT(i), i);
	
		if(!DLH[i]){
			printf("%d TAKE RIGHT FORKS AND STARTS EATING\n", i);
			eating();
			printf("%d RELEASES RIGHT FORK\n", i);
			put_fork(RIGHT(i), i);
		}else{
			put_fork(RIGHT(i), i);
			DLH[i] = 0;
			sem_post(&ck);
		}
		
		printf("%d RELEASES LEFT FORK\n", i);
		put_fork(i, i);
	}

	free(arg);
	return NULL;
}


// Finding cycles in the Graph G..
int vis[2*NOP];
bool iscycle;

void dfs(int s){
	if(vis[s]){
		if(vis[s] == 1) iscycle = 1;
		return;
	}
	vis[s] = 1;
	
	for(int i = 0; i < 2*NOP; ++i){
		if(G[s][i]) dfs(i);
	}

	vis[s] = 2;
}


int main(int argc, char *argv[]){
	pthread_t th[NOP];
	int i;

	sem_init(&ck, 0, 0);

	for(i = 0; i < NOP; ++i){
		sem_init(&F[i], 0,  1);
	}

	for(i = 0; i < NOP; ++i){
		int *a = (int *)malloc(sizeof(int));
		*a = i;
		pthread_create(&th[i], NULL, philospher, a);
	}

	while(1){
		sleep(2);

		pthread_mutex_lock(&m);
		memset(vis, 0, sizeof(vis));
		dfs(iscycle = 0);
		pthread_mutex_unlock(&m);
		
		if(iscycle == 1){
			printf("\n\nDEADLOCK\n\n\n");
			i = rand()%5;
			DLH[i] = 1;
			sem_post(&F[RIGHT(i)]);
			sem_wait(&ck);
			sem_wait(&F[RIGHT(i)]); // Assuming it won't need serializing..
		}
	}

}
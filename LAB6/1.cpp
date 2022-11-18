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

int AC[NOP]; //ACTIVITY
sem_t S[NOP];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void think(){
	sleep(1 + rand()%5); // random second between 1 to 5
}

void eating(){
	sleep(1 + rand()%3); // random second between 1 to 3 
}

void test(int i){
	if(AC[i] == HUNGRY && AC[LEFT(i)] != EATING && AC[RIGHT(i)] != EATING){
		AC[i] = EATING;
		sem_post(&S[i]);
	}
}

void take_fork(int i){
	pthread_mutex_lock(&m);
	AC[i] = HUNGRY;
	test(i);
	pthread_mutex_unlock(&m);
	sem_wait(&S[i]);
}

void put_fork(int i){
	pthread_mutex_lock(&m);
	AC[i] = THINKING;
	test(LEFT(i));
	test(RIGHT(i));
	pthread_mutex_unlock(&m);
}

void* philospher(void *arg){
	int i = *(int *)arg;
	srand(time(NULL) + i);
	
	while(1){
		printf("%d STARTS THINKING\n", i);
		think();

		printf("%d BECOMES HUNGRY\n", i);
		take_fork(i);
		
		printf("%d TAKE BOTH FORKS AND STARTS EATING\n", i);
		eating();
		
		printf("%d ENDS EATING AND RELEASES BOTH FORKS\n", i);
		put_fork(i);
	}

	free(arg);
	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t th[NOP];
	int i;

	for(i = 0; i < NOP; ++i){
		sem_init(&S[i], 0,  0);
	}

	for(i = 0; i < NOP; ++i){
		int *a = (int *)malloc(sizeof(int));
		*a = i;
		pthread_create(&th[i], NULL, philospher, a);
	}

	pthread_join(th[0], NULL);
}
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int NOP = 4; // # of Processes
const int T = 1;
const int BUF = 20;

void make(int round, int priority, double prob, double sleeptime){
	char cround[BUF], cpriority[BUF], csleeptime[BUF], cprob[BUF];
	sprintf(cround, "%d", round);
	sprintf(cpriority, "%d", priority);
	sprintf(cprob, "%lf", prob);
	sprintf(csleeptime, "%lf", sleeptime);

	execlp("xterm", "xterm", "-e", "./process", cround, cpriority, cprob, csleeptime, (char *)NULL);
}

void random(int seed){
	int round, priority;
	double prob ,sleeptime;

	srand(time(NULL) + seed);
	round = rand()%100000;
	priority = 1 + rand()%10;
	prob = (double)(rand()%80000)/100000;
	sleeptime = rand()*1.0 / RAND_MAX; // seconds
	make(round, priority, prob, sleeptime);
}


int main(int argc, char* argv[]){
	int i, pid;

	for(i = 0; i < NOP; ++i){
		printf("%d\n", i);
		pid = fork();

		if(pid == 0){ //child's process
			srand(time(NULL) + i);
			if(i < 2){
				make(100, 1 + rand()%5, 0.3, 0.5);
			}else{
				make(40, 6 + rand()%5, 0.7, 1.5);
			}
		}
		sleep(T);
	}

	// waiting for the all child to execute..
	for(i = 0; i < NOP; ++i){
		wait(NULL);
	}

}
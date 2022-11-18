#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


#define ERROR printf("ERROR\n");
#define N 1005


int* merge_sort(int* a, int l, int r){
	if(l == r) return a;

	int mid = (l + r)/2, i;
	
	int fd[2];
	if(pipe(fd) < 0){
		ERROR; exit(0);
	}

	int pid;
	if((pid = fork()) < 0){
		ERROR; exit(0);
	}

	if(!pid){
		close(fd[0]);
		int *x = merge_sort(a, l, mid);
		for(i = l; i <= mid; ++i){
			write(fd[1], &a[i], sizeof(int));
		}
		close(fd[1]);
		exit(0);
	}

	int left = mid-l+1, right = r-mid;
	int t[left], t1[right];
	
	close(fd[1]);
	merge_sort(a, mid+1, r);
	for(i = mid+1; i <= r; ++i){
		t1[i-mid-1] = a[i];
	}
	wait(NULL);


	for(i = l; i <= mid; ++i){
		if(read(fd[0], &t[i-l], sizeof(int)) < 0){
			ERROR; exit(0);
		}
	}

	i = 0; int j = 0, k = l;
	while(i < left || j < right){
		if(i == left || (j < right && t[i] > t1[j])) a[k++] = t1[j++];
		else a[k++] = t[i++];
	}

	return a;
}

int main(int argc, char* argv[]){
	FILE *fp = fopen(argv[1], "r");
	FILE *fp1 = fopen("Output.txt", "w");
	if(fp == NULL) ERROR;

	int n, arr[N];
	fscanf(fp, "%d", &n);
	if(n == 0) return 0;

	for(int i = 0; i < n; ++i){
		fscanf(fp, "%d", &arr[i]);
	}

	int *ans = merge_sort(arr, 0, n-1);
	for(int i = 0; i < n; ++i){
		fprintf(fp1, "%d ", ans[i]);
	}
	// printf("\n");
}
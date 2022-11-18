#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
	const int n = 100;
	srand(time(NULL));

	FILE *fp = fopen("Input.txt", "w");
	fprintf(fp, "%d ", n);
	int i;
	for(i = 0; i < n; ++i){
		fprintf(fp, "%d ", rand()%1000);
	}
	
	fclose(fp);
}
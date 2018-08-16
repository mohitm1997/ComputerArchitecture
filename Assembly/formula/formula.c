#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/time.h>
#include "nCr.h"
#include "formula.h"

int main(int argc, char **argv){
	
	//chekcs validity of arguments
	if(argc!=2){
		fprintf(stderr,"Error: Invalid number of arguments\n");
		return 1;
	}
	if(strcmp(argv[1],"-h")==0){
		printf("Usage: ./formula <positive integer>\n");
		return 1;
	}

	int i;
	for(i=0;i<strlen(argv[1]);i++){
		if(!isdigit(argv[1][i])){
			fprintf(stderr,"Error: Input must be a positive integer\n");
			return 1;
		}
	}	

	//gets the start time
	struct timeval start, end;
	gettimeofday(&start, NULL);

	//calculates and prints the expansion of the polynomial using nCr
	int n = atoi(argv[1]);
	int r;
	for(r=0; r<=n; r++){
		int result = nCr(n,r);
		if(result == 0){
			fprintf(stderr, "Error: Input causes overflow\n");
			return 1;
		}

		if(result == 1 && r == 0)
		{
			printf("(1 + x)^%d = %d",n,result);
		}
		else
		{
			printf(" + %d*x^%d",result,n);
		}
	}
	printf(" \n");

	//gets end time of program and prints time in mivroseonvds
	gettimeofday(&end, NULL);
	printf("Time elapsed: %ld microseconds\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

	return 0;

}
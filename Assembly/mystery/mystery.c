#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mystery.h"	

int num[200];

//adds two numbers together (2 int arguments, int output)
int add(int num1, int num2)
{
	return num1 + num2;
}

//calculates the nth fibonacci number
int dothething(int n)
{
	if(num[n] == -1)
	{
		if(n == 0)
		{
			num[n] = 0;
		}
		else if(n == 1)
		{
			num[n] = 1;
		}
		else
		{
			num[n] = add(dothething(n-2),dothething(n-1));
		}
	}
	return num[n];
}

int main(int argc, char** argv)
{
	int n = atoi(argv[1]);
	int i = 0;
	while(i <= 199)
	{
		num[i] = -1;
		i++;
	}
	printf("Value:   %d\n", dothething(n));
	return 0;
}
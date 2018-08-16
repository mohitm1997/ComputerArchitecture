#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "format.h"

// convert converts a string input to a decimal int and returns it
int convert (char* input)
{
	int pos = 0;
	int toReturn = 0;

	while(pos < strlen(input))
	{
		toReturn = toReturn * 2 + (input[pos] - '0');
		pos++;
	}

	return toReturn;
}

// getSign gets the sign(positive/negative) of the input string and returns it
int getSign(char* input)
{
	if(input[0] == '1') return -1;
	return 1;
}

// twosComplement finds the decimal representation of a binary input string using the twos complement method and returns it
int twosComplement(char* input)
{
	int sign = 1;
	int curr = 0;

	if(input[curr] == '1') 
	{	
		sign = -1;
		while(curr < strlen(input))
		{
			if(input[curr] == '0') input[curr] = '1';
			else input[curr] = '0';
			curr++;
		}
	}
	return (convert(input) + (sign == -1 ? 1:0)) * sign;
}

// mantissaConvert converts the mantissa input string into a decimal float and returns it
float mantissaConvert(char* input)
{
	float toReturn = 0;
	float currVal = .5;
	int pos = 0;

	while(pos < strlen(input))
	{
		if(input[pos] == '1') toReturn += currVal;
		currVal /= 2;
		pos++;
	}
	return toReturn;
}

// power calculates and returns base raised to the power exponent
float power(float base, int exponent)
{
	if(exponent == 0) return 1;

	if(exponent < 0)
	{
		base = .5;
		exponent *= -1;
	}

	float result = base;

	while(exponent > 1)
	{
		result *= base;
		exponent--;
	}
	return result;
}

// ieee754 converts and returns and input string to a decimal float representation based on ieee754
float ieee754(char* input, int sign)
{
	char* exponent = (char*)malloc(sizeof(char) * 9);
	char* mantissa = (char*)malloc(sizeof(char) * 24);

	strncpy(exponent, input + 1, 8);
	strncpy(mantissa, input + 9, 23);
	exponent[8] = '\0';
	mantissa[23] = '\0';

	int expoConv = convert(exponent) - 127;
	float mantConv = 1 + mantissaConvert(mantissa);

	if(expoConv == 128)
	{
		if(sign == -1)
		{
			if(mantConv - 1 == 0) printf("ninf\n");
			else printf("NaN\n");
			exit(1);
		}
		else
		{
			if(mantConv - 1 == 0) printf("pinf\n");
			else printf("NaN\n");
			exit(1);
		}		
	}

	if(expoConv == -127)
	{
		if(mantConv - 1 == 0) mantConv = 0;
		else
		{
			expoConv++;
			mantConv--;
		}
	}
	
	float result = sign * power(2, expoConv) * mantConv;
	free(exponent);
	free(mantissa);
	return result;
}

// formatResult prints the float input in scientific notation
void formatResult(float num, int sign)
{
	int expo = 0;
	if(sign == -1) num *= -1;

	if(num == 0)
	{
		sign == -1 ? printf("-0.0e0\n") : printf("0.0e0\n");
		return;
	}

	while(num > 10)
	{
		num /= 10;
		expo++;
	}
	while(num < 1)
	{
		num *= 10;
		expo--;
	}

	sign == -1 ? printf("-%fe%d\n", num, expo) : printf("%fe%d\n", num, expo);
	return;
} 

// binaryCheck checks if the input string is a valid binary input
int binaryCheck(char* num)
{
	int pos = 0;

	while(pos < strlen(num))
	{
		if(num[pos] != '1' && num[pos] != '0') return 0;
		pos++;
	}
	return 1;
}

int main(int argc, char** argv)
{
	if(argc > 3)
	{
		fprintf(stderr, "ERROR: Too many arguments\n");
		return 1;
	}
	else if(argc < 3)
	{
		fprintf(stderr, "ERROR: Too few arguments\n");
		return 1;
	}

	if(binaryCheck(argv[1]) == 0)
	{
		fprintf(stderr, "ERROR: Input is not binary\n");
		return 1;
	}

	if(strlen(argv[1]) != 32)
	{
		fprintf(stderr, "ERROR: Input sequence should be 32 bits\n");
		return 1;
	}

	if(strcmp(argv[2],"int") != 0 && strcmp(argv[2],"float") != 0)
	{
		fprintf(stderr, "ERROR: Invalid type\n");
		return 1;
	}

	int intResult;
	float floatResult;

	if(strcmp(argv[2],"int") == 0)
	{
		intResult = twosComplement(argv[1]);
		printf("%d\n", intResult);
	}
	else if(strcmp(argv[2],"float") == 0)
	{
		int sign = getSign(argv[1]);
		floatResult = ieee754(argv[1], sign);
		formatResult(floatResult, sign);
	}

	return 0;
}
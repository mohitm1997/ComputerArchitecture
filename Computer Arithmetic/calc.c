#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "calc.h"

// createNumber creates a Number struct and allocates memory for the integer array
Number* createNumber()
{
	Number * number = (Number*)malloc(sizeof(Number));
	number->numbers = (int*)malloc(sizeof(int)*2);
	return number;
}

// getOperation parses the operation being performed from the string parameter and returns it
Operation getOperation (char * sign)
{
	if(strlen(sign) > 1)
	{
		return INVALID;
	}
	else if(*sign == '+')
	{
		return ADDITION;
	}
	else if(*sign == '-')
	{
		return SUBTRACTION;
	}
	else if(*sign == '*')
	{
		return MULTIPLICATION;
	}
	else
	{
		return INVALID;
	}
}

// getBase returns the base (binary,decimal,octal,hex, or invalid) of the char passed through
int getBase(char num)
{
	if(num == 'b' || num == 'B')
		return 2;
	else if(num == 'd' || num == 'D')
		return 10;
	else if(num == 'o' || num == 'O')
		return 8;
	else if(num == 'x' || num == 'X')
		return 16;
	else
		return -1;			
}

// getLength returns the length of the output string using the paramters: the int output number and the desired output base
int getLength(int num, int base)
{
	int length = 0;
	if(num < 0)
	{ 
		num *= -1;
		length++;
	}
	int currPlace = 1;
	while(currPlace <= num && currPlace > 0)
	{
		length++;
		currPlace *= base;
	}
	return length;
}

// hexConv converts the hex characters A-F/a-f in current to an integer representation and returns it
int hexConv(char current)
{
  if (current >= 65 && current <= 70 ){
    return (current - 55);
  }
  if (current >= 97 && current <= 102){
    return (current - 87);
  }
  return 0;
}

// reverse reverses the string that is passed through and returns it
char* reverse (char* string)
{
	int j = strlen(string) - 1;
	char temp;
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		temp = string[i];
		string[i] = string[j];
		string[j] = temp;
		j--;
		if(j == (strlen(string)-1)/2) break;
	}
	return string;
}

// convertResult converts an int decimal number to an ASCII string representing the number in the desired base and returns it
char* convertResult(int num, char base)
{
	int numBase = getBase(base);
	char* toReturn = (char*)malloc(sizeof(char) * (getLength(num, numBase) + 3));
	char sign = '+';
	int i = 0 ;

	if(num < 0)
	{ 
		sign = '-';
		num *= -1;
	}
	do
	{
		if(numBase == 16 && (num % numBase) > 9) toReturn[i] = ((num % numBase) + '0') + 7;
		else toReturn[i] = (num % numBase) + '0';
		num /= numBase;
		i++;
	}while(num != 0);

	toReturn[i] = base;
	if(sign == '-') toReturn[++i] = '-';
	toReturn[++i] = '\0';

	toReturn = reverse(toReturn);
	return toReturn;
}

// convert converts an ASCII string to an int in decimal and returns it 
int convert(char* num)
{
	int sign = 1;
	int toReturn = 0;
	int pos = 0;
	if(num[0] == '-') 
	{
		sign = -1; 
		pos = 1;
	}
	int base = getBase(num[pos++]);

	while(pos < strlen(num))
	{
		int curr;
		if(isxdigit(num[pos]) && !isdigit(num[pos]))
			curr = hexConv(num[pos]);
		else curr = num[pos] - '0';
		toReturn = toReturn * base + curr;
		pos++;
	}
	toReturn *= sign;
	return toReturn;
}

// checkNum checks if the input number is valid and returns 1 if valid and -1 if invalid
int checkNum(char* num)
{	
	int pos = 0;
	if(num[pos] == '-')	
		pos++;

	int base = getBase(num[pos]);

	pos++;

	if(strlen(num) == 1) return -1;

	if(base == -1) return -1;
	else if(base == 2 || base == 10 || base == 8)
	{
		while(pos < strlen(num))
		{
			if(!isdigit(num[pos]) || num[pos] >=  base + '0') return -1;
			pos++;
		}
	}
	else if(base == 16)
	{
		while(pos < strlen(num))
		{
			if(!isxdigit(num[pos])) return -1;
			pos++;
		}
	}
	return 1;
}

int main(int argc, char **argv)
{
	if(argc > 5)
	{
		fprintf(stderr, "ERROR: too many arguments\n");
		return 1;
	}
	if(argc < 5)
	{
		fprintf(stderr, "ERROR: too few arguments\n");
		return 1;
	}
	
	Number * number = createNumber();
	number->operation = getOperation(argv[1]);
	if(number->operation == INVALID)
	{
		fprintf(stderr, "ERROR: Invalid operation\n");
		return 1;
	}

	if(checkNum(argv[2]) == -1)
	{
		fprintf(stderr, "ERROR: Invalid number: %s\n", argv[2]);
		return 1;
	}

	if(checkNum(argv[3]) == -1)
	{
		fprintf(stderr, "ERROR: Invalid number: %s\n", argv[3]);
		return 1;
	}

	number->numbers[0] = convert(argv[2]);
	number->numbers[1] = convert(argv[3]);
	if(strlen(argv[4]) > 1 || getBase(*argv[4]) == -1)
	{
		fprintf(stderr, "ERROR: Invalid output base\n");
		return 1;
	}

	char* result;
	if(number->operation == ADDITION) result = convertResult((number->numbers[0] + number->numbers[1]), *argv[4]);
	else if(number->operation == SUBTRACTION) result = convertResult((number->numbers[0] - number->numbers[1]), *argv[4]);
	else if(number->operation == MULTIPLICATION) result = convertResult((number->numbers[0] * number->numbers[1]), *argv[4]);

	printf("%s\n", result);

	free(result);
	free(number->numbers);
	free(number);
	return 0;
}
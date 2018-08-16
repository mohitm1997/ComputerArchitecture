#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "y86emul.h"

//registers
#define eax 0
#define ecx 1
#define edx 2
#define ebx 3
#define esp 4
#define ebp 5
#define esi 6
#define edi 7

//converts decimal values to hex
char* decimalToHex(int number)
{
	char* toReturn = (char*)malloc(3);
	if(number == 0) return "00";
	toReturn[0] = '0';
	int temp;
	int i = 1;

	while(number != 0)
	{
		temp = number % 16;
		if(temp < 10) toReturn[i] = 48+temp;
		else toReturn[i] = 87+temp;
		i--;
		number /= 16;
	}
	toReturn[2] = '\0';
	return toReturn;
}

//computes powers
double power(double base, int exponent)
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

//converts a hex value to a decimal value
int hexToDecimal(char* value)
{
	int toReturn = 0;
	int len = strlen(value) - 1;
	int i = 0;
	int num;
	while(len >= 0)
	{
		if(isxdigit(value[i]) && !isdigit(value[i]))
		{
			switch(value[i])
			{
				case 'a': 
					num = 10;
					break;
				case 'b':
					num = 11;
					break;
				case 'c':
					num = 12;
					break;
				case 'd':
					num = 13;
					break;
				case 'e':
					num = 14;
					break;
				case 'f':
					num = 15;
					break;
				default:
					break;
			}
		}
		else num = value[i] - '0';
		toReturn += num * power(16,len);
		len--;
		i++;
	}
	return toReturn;
}

//converts a twos complement binary value to a signed decimal integer
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

	int pos = 0;
	int toReturn = 0;

	while(pos < strlen(input))
	{
		toReturn = toReturn * 2 + (input[pos] - '0');
		pos++;
	}

	//printf("here: %d\n", (toReturn + (sign == -1 ? 1:0)) * sign);
	return (toReturn + (sign == -1 ? 1:0)) * sign;
}

//converts hex values to binary
char* hexToBinary(char* num)
{
	char* toReturn = (char*)malloc(33);
	toReturn[32] = '\0';
	int i = 0;	
	int k = 7;
	while(k >= 0)
	{
		switch(num[i])
		{
			case '0': strcat(toReturn,"0000");
				break;
			case '1': strcat(toReturn,"0001");
				break;
			case '2': strcat(toReturn,"0010");
				break;
			case '3': strcat(toReturn,"0011");
				break;
			case '4': strcat(toReturn,"0100");
				break;
			case '5': strcat(toReturn,"0101");
				break;
			case '6': strcat(toReturn,"0110");
				break;
			case '7': strcat(toReturn,"0111");
				break;
			case '8': strcat(toReturn,"1000");
				break;
			case '9': strcat(toReturn,"1001");
				break;
			case 'a': strcat(toReturn,"1010");
				break;
			case 'b': strcat(toReturn,"1011");
				break;
			case 'c': strcat(toReturn,"1100");
				break;
			case 'd': strcat(toReturn,"1101");
				break;
			case 'e': strcat(toReturn,"1110");
				break;
			case 'f': strcat(toReturn,"1111");
				break;
			default: break;
		}
		i++;
		k--;
	}
	return toReturn;
}

//converts a little endian 32-bit hex value to a big endian 32 bit binary value
char* bigEndianBinary(emulator* processor, int* memory)
{
	char* value = (char*)malloc(9);
	processor -> ip++;
	char* temp1 = decimalToHex(memory[processor -> ip]);
	processor -> ip++;
	char* temp2 = decimalToHex(memory[processor -> ip]);
	processor -> ip++;
	char* temp3 = decimalToHex(memory[processor -> ip]);
	processor -> ip++;
	char* temp4 = decimalToHex(memory[processor -> ip]);
	strcpy(value, temp4);
	strcat(value, temp3);
	strcat(value, temp2);
	strcat(value, temp1);
	value[8] = '\0';
	//printf("hey %s\n", value);
	char* toReturn = hexToBinary(value);
	free(value);
	return toReturn;
}

//returns the Status as a string for printing
char* stateToString(emulator* processor)
{
	switch(processor -> currState)
	{
		case AOK: return "AOK";
		case HLT: return "HLT";
		case ADR: return "ADR";
		case INS: return "INS";
		default: return NULL;
	}
}

//sets condition flags based on value in rB
void flags(emulator *processor, int rB)
{
	processor -> ZF = (processor -> registers[rB] == 0) ? 1:0;
	processor -> SF = (processor -> registers[rB] < 0) ? 1:0;
	processor -> OF = (processor -> registers[rB] > 2147483647 || processor -> registers[rB] < -2147483647) ? 1:0;
}

//checks if address is in bounds of memory
int checkAddress(int adr, emulator* processor)
{
	if(adr > processor -> size)
	{
		processor -> currState = ADR;
		return 1;
	}
	return 0;
}

//executes instrcutions until the HLT instruction is reached or until there is an invalid address or instruction
void execute(emulator* processor, int* memory)
{
	char* inst = decimalToHex(memory[processor -> ip]);
	while(processor -> currState == AOK)
	{
	//printf("inst: %s\n", inst);

		if(strcmp(inst,"00") == 0) //nop
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"10") == 0) //halt
		{
			processor -> currState = HLT;
		}
		else if(strcmp(inst,"20") == 0) //rrmovl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] = processor -> registers[rA];
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"30") == 0) //irmovl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int reg = inst[1] - '0';
			int value = twosComplement(bigEndianBinary(processor, memory));
			processor -> registers[reg] = value;
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"40") == 0) //rmmovl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			//printf("%d\n", displacement);
			if(checkAddress(processor -> registers[rB] + displacement, processor) == 1) break;
			memory[processor -> registers[rB] + displacement] = processor -> registers[rA];
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"50") == 0 || strcmp(inst,"e0") == 0) //mrmovl & movsbl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(processor -> registers[rB] + displacement, processor) == 1) break;
			processor -> registers[rA] = memory[processor -> registers[rB] + displacement];
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"60") == 0) //addl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] += processor -> registers[rA];
			flags(processor, rB);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"61") == 0) //subl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] -= processor -> registers[rA];
			flags(processor, rB);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);			
		}
		else if(strcmp(inst,"62") == 0) //andl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] &= processor -> registers[rA];
			flags(processor, rB);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"63") == 0) //xorl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] ^= processor -> registers[rA];
			flags(processor, rB);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);			
		}
		else if(strcmp(inst,"64") == 0) //mull
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			processor -> registers[rB] *= processor -> registers[rA];
			flags(processor, rB);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);						
		}
		else if(strcmp(inst,"65") == 0) //cmpl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int val = processor -> registers[rB] - processor -> registers[rA];
			processor -> ZF = val == 0 ? 1:0;
			processor -> SF = val < 0 ? 1:0;
			if((processor -> registers[rB] > 0 && processor -> registers[rA] < 0 && (processor -> registers[rB] - processor -> registers[rA]) < 0)||
				(processor -> registers[rB] < 0 && processor -> registers[rA] > 0 && (processor -> registers[rB] - processor -> registers[rA]) > 0)) processor -> OF = 1;
			else processor -> OF = 0;
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"70") == 0) //jmp
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			processor -> ip = dest;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"71") == 0) //jle
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if(processor -> ZF == 1 || (processor -> SF^processor -> OF) == 1) processor -> ip = dest;	
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"72") == 0) //jl
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if((processor -> SF^processor -> OF) == 1) processor -> ip = dest;
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"73") == 0) //je
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if(processor -> ZF == 1) processor -> ip = dest;
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"74") == 0) //jne
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if(processor -> ZF == 0) processor -> ip = dest;
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"75") == 0) //jge
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if((processor -> SF^processor -> OF) == 0) processor -> ip = dest;
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);			
		}
		else if(strcmp(inst,"76") == 0) //jg
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			if((processor -> SF^processor -> OF) == 0 && processor -> ZF == 0) processor -> ip = dest;
			else processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"80") == 0) //call
		{
			int dest = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(dest, processor) == 1) break;
			processor -> registers[esp] -= 4;
			memory[processor -> registers[esp]] = ++processor -> ip;
			//printf("call: %d\n", memory[processor -> registers[esp]]);

			processor -> ip = dest;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"90") == 0) //return
		{
			processor -> ip = memory[processor -> registers[esp]];
			//printf("return: %d\n", memory[processor -> registers[esp]]);

			processor -> registers[esp] += 4;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"a0") == 0) //pushl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			processor -> registers[esp] -= 4;
			memory[processor -> registers[esp]] = processor -> registers[rA];
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"b0") == 0) //popl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			//printf("%d\n",  memory[processor -> registers[esp]]);
			processor -> registers[rA] = memory[processor -> registers[esp]];
			//printf("%d\n", rA);
			processor -> registers[esp] += 4;
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);			
		}
		else if(strcmp(inst,"c0") == 0) //readb
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(processor -> registers[rA] + displacement, processor) == 1) break;
			char readIn;
			scanf("%c", &readIn);
			memory[processor -> registers[rA] + displacement] = (int)readIn;
			processor -> ZF = memory[processor -> registers[rA] + displacement] < 0 ? 1:0; 
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);			
		}
		else if(strcmp(inst,"c1") == 0) //readl
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(processor -> registers[rA] + displacement, processor) == 1) break;
			int readIn;
			scanf("%d",&readIn);
			memory[processor -> registers[rA] + displacement] = readIn;
			processor -> ZF = memory[processor -> registers[rA] + displacement] < 0 ? 1:0; 
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"d0") == 0) //writeb
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(processor -> registers[rA] + displacement, processor) == 1) break;
			printf("%c", (char)memory[processor -> registers[rA] + displacement]);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else if(strcmp(inst,"d1") == 0) //writel
		{
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
			int rA = inst[0] - '0';
			int displacement = twosComplement(bigEndianBinary(processor, memory));
			if(checkAddress(processor -> registers[rA] + displacement, processor) == 1) break;
			printf("%d", memory[processor -> registers[rA] + displacement]);
			processor -> ip++;
			inst = decimalToHex(memory[processor -> ip]);
		}
		else
		{
			fprintf(stderr, "ERROR: \"%s\" is an Invalid Instruction", inst);
			processor -> currState = INS;
		}
			//printf("	mem 6008: %d ", memory[6008]);
			//printf("0: %d ", processor -> registers[0]);
			//printf("1: %d ", processor -> registers[1]);
			//printf("2: %d ", processor -> registers[2]);
			//printf("3: %d ", processor -> registers[3]);
			//printf("4: %d ", processor -> registers[4]);
			//printf("5: %d ", processor -> registers[5]);
			//printf("6: %d ", processor -> registers[6]);
			//printf("7: %d\n", processor -> registers[7]);
	}

	if(processor -> currState == ADR)
	{
		fprintf(stderr, "ERROR: Invalid address");
	}
	free(inst);
}

int main(int argc, char** argv)
{
	//checks argument number
	if(argc != 2)
	{
		fprintf(stderr, "ERROR: Invalid number of arguments.\n");
		return 1;
	}

	//help flag: shows usage instructions
	if(strcmp(argv[1],"-h") == 0)
	{
		printf("USAGE: ./y86emul <y86 input file>\n");
		return 0;
	}

	//opens file specified in argument for reading
	FILE *file = fopen(argv[1],"r");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: File \"%s\" not found.\n", argv[1]);
		return 1;
	}

	//gets size directive and allocates memory accordingly
	char* directive = (char*)malloc(8);
	int size;
	fscanf(file,"%s\t%x",directive, &size);

	if(strcmp(directive, ".size") != 0)
	{
		fprintf(stderr, "ERROR: No size directive found");
		return 1;
	}

	emulator *processor = (emulator*)malloc(sizeof(emulator));
	processor -> size = size;
	int* memory = (int*)malloc(size*sizeof(int));

	//gets other directives and stores values at specified locations in memory
	int address;
	int value;
	while(fscanf(file,"%s", directive) != EOF)
	{
		if(strcmp(directive, ".text") == 0)
		{
			int start = 0;
			fscanf(file,"\t%x", &start);
			processor -> ip = start;
			while(fscanf(file,"\t%2x", &value) == 1)
			{
				memory[start] = value;
				start++;
			}
		}
		else if(strcmp(directive, ".byte") == 0)
		{
			fscanf(file, "\t%x\t%x", &address, &value);
			memory[address] = value;
		}
		else if(strcmp(directive, ".long") == 0)
		{
			fscanf(file, "\t%x\t%d", &address, &value);
			memory[address] = value;
		}
		else if(strcmp(directive, ".string") == 0)
		{
			char currChar;
			fscanf(file, "\t%x", &address);
			fscanf(file, " %c", &currChar);
			do
			{
				fscanf(file, "%c", &currChar);
				memory[address++] = currChar + 0;
			}while(currChar != '\"');

		}
	}

	//execution
	processor -> currState = AOK;
	execute(processor, memory);
	printf("End Status: %s\n", stateToString(processor));
	fclose(file);
	free(memory);
	free(directive);
	return 0;
}		
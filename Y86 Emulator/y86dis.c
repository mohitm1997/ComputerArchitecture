#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char* decimalToHex(int number)
{
	char* toReturn = (char*)malloc(3);
	if(number == 0)
	{
		return "00";
	}
	toReturn[0] = '0';
	int temp;
	int i = 1;

	while(number != 0)
	{
		temp = number % 16;
		if(temp < 10)
		{
			toReturn[i] = 48+temp;
		}
		else
		{
			toReturn[i] = 87+temp;
		}
		i--;
		number /= 16;
	}
	toReturn[2] = '\0';
	return toReturn;
}

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

char* getValue(int curr, int* memory)
{
	char* value = (char*)malloc(9);
	curr++;
	char* temp1 = decimalToHex(memory[curr]);
	curr++;
	char* temp2 = decimalToHex(memory[curr]);
	curr++;
	char* temp3 = decimalToHex(memory[curr]);
	curr++;
	char* temp4 = decimalToHex(memory[curr]);
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

char* getReg(int n)
{
	switch(n)
	{
		case 0: return "%eax";
			break;
		case 1: return "%ecx";
			break;
		case 2: return "%edx";
			break;
		case 3: return "%ebx";
			break;
		case 4: return "%esp";
			break;
		case 5: return "%ebp";
			break;
		case 6: return "%esi";
			break;
		case 7: return "%edi";
			break;
		default: return "err";
	}
}

void disassemble(FILE *file, int* memory, int start, FILE *fp)
{
	int curr = start;
	char* inst = decimalToHex(memory[curr]);
	int end = 0;
	while(!end)
	{
		if(strcmp(inst,"00") == 0)
		{
			printf("%-5d: NOP\n",curr);
			fprintf(fp,"%-5d: NOP\n",curr);
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"10") == 0)
		{
			printf("%-5d: HALT\n", curr);
			fprintf(fp,"%-5d: HALT\n", curr);
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"20") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: rrmovl %s, %s\n",curr - 1, getReg(rA),getReg(rB));
			fprintf(fp,"%-5d: rrmovl %s, %s\n",curr - 1, getReg(rA),getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"30") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[1] - '0';
			int value = twosComplement(getValue(curr, memory));
			printf("%-5d: irmovl $%d, %s\n",curr - 1,value,getReg(reg));
			fprintf(fp,"%-5d: irmovl $%d, %s\n",curr - 1,value,getReg(reg));
			curr += 5;
			inst = decimalToHex(memory[curr]);	
		}
		else if(strcmp(inst,"40") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: rmmovl %s, %d(%s)\n", curr - 1, getReg(rA), displacement, getReg(rB));
			fprintf(fp,"%-5d: rmmovl %s, %d(%s)\n", curr - 1, getReg(rA), displacement, getReg(rB));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"50") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: mrmovl %d(%s), %s\n", curr - 1, displacement, getReg(rB), getReg(rA));
			fprintf(fp,"%-5d: mrmovl %d(%s), %s\n", curr - 1, displacement, getReg(rB), getReg(rA));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "60") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: addl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: addl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "61") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: subl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: subl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "62") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: andl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: andl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "63") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: xorl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: xorl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "64") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: mull   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: mull   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"65") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			printf("%-5d: cmpl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			fprintf(fp,"%-5d: cmpl   %s, %s\n", curr - 1, getReg(rA), getReg(rB));
			curr++;
			inst = decimalToHex(memory[curr]);	
		}
		else if(strcmp(inst,"70") == 0)
		{
			int dest = twosComplement(getValue(curr,memory));
			printf("%-5d: jmp    %d\n", curr, dest);
			fprintf(fp,"%-5d: jmp    %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"71") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: jle    %d\n", curr, dest);
			fprintf(fp,"%-5d: jle    %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"72") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: jl     %d\n", curr, dest);
			fprintf(fp,"%-5d: jl     %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"73") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: je     %d\n", curr, dest);
			fprintf(fp,"%-5d: je     %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"74") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: jne    %d\n", curr, dest);
			fprintf(fp,"%-5d: jne    %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"75") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: jge    %d\n", curr, dest);
			fprintf(fp,"%-5d: jge    %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"76") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: jg     %d\n", curr, dest);
			fprintf(fp,"%-5d: jg     %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"80") == 0)
		{
			int dest = twosComplement(getValue(curr, memory));
			printf("%-5d: call   %d\n", curr, dest);
			fprintf(fp,"%-5d: call   %d\n", curr, dest);
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"90") == 0)
		{
			printf("%-5d: return\n", curr);
			fprintf(fp,"%-5d: return\n", curr);
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"a0") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			printf("%-5d: pushl  %s\n",curr - 1, getReg(reg));
			fprintf(fp,"%-5d: pushl  %s\n",curr - 1, getReg(reg));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"b0") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			printf("%-5d: popl   %s\n",curr - 1, getReg(reg));
			fprintf(fp,"%-5d: popl   %s\n",curr - 1, getReg(reg));
			curr++;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "c0") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: readb  %d(%s)\n",curr - 1, displacement, getReg(reg));
			fprintf(fp,"%-5d: readb  %d(%s)\n",curr - 1, displacement, getReg(reg));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "c1") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: readl  %d(%s)\n",curr - 1, displacement, getReg(reg));
			fprintf(fp,"%-5d: readl  %d(%s)\n",curr - 1, displacement, getReg(reg));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "d0") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: writeb %d(%s)\n",curr - 1, displacement, getReg(reg));
			fprintf(fp,"%-5d: writeb %d(%s)\n",curr - 1, displacement, getReg(reg));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst, "d1") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int reg = inst[0] - '0';
			int displacement = twosComplement(getValue(curr,memory));
			printf("%-5d: writel %d(%s)\n",curr - 1, displacement, getReg(reg));
			fprintf(fp,"%-5d: writel %d(%s)\n",curr - 1, displacement, getReg(reg));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else if(strcmp(inst,"e0") == 0)
		{
			curr++;
			inst = decimalToHex(memory[curr]);
			int rA = inst[0] - '0';
			int rB = inst[1] - '0';
			int displacement = twosComplement(getValue(curr, memory));
			printf("%-5d: movsbl %d(%s), %s\n", curr - 1, displacement, getReg(rB), getReg(rA));
			fprintf(fp,"%-5d: movsbl %d(%s), %s\n", curr - 1, displacement, getReg(rB), getReg(rA));
			curr += 5;
			inst = decimalToHex(memory[curr]);
		}
		else
		{
			end = 1;
		}
	}
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "\x1b[31mERROR: Invalid number of arguments.\x1b[0m\n");
		return 1;
	}

	if(strcmp(argv[1], "-h") == 0)
	{
		printf("\x1b[36mUSAGE: ./y86dis <y86 input file>\x1b[0m\n");
		return 0;
	}

	FILE *file = fopen(argv[1],"r");
	if(file == NULL)
	{
		fprintf(stderr, "\x1b[31mERROR: File \"%s\" not found.\x1b[0m\n", argv[1]);
		return 1;
	}

	char* directive = (char*)malloc(8);
	int size;
	fscanf(file,"%s\t%x",directive, &size);

	if(strcmp(directive, ".size") != 0)
	{
		fprintf(stderr, "\x1b[31mERROR: No size directive found.\x1b[0m");
		return 1;
	}

	int* memory = (int*)malloc(size*sizeof(int));
	int value;
	int found = 0;
	int start = 0;

	while(fscanf(file,"%s", directive) != EOF)
	{
		if(strcmp(directive, ".text") == 0)
		{
			fscanf(file,"\t%x", &start);
			int i = start;
			while(fscanf(file,"\t%2x", &value) == 1)
			{
				memory[i] = value;
				i++;
			}
			found = 1;
			memory[i] = 255;
		}
	}

	if(!found)
	{
		fprintf(stderr, "ERROR: No .text directive found\n");
		return 1;
	}

	FILE *fp = fopen("assembly.txt", "w");
	printf("\n\x1b[32m%s (disassembled):\x1b[0m\n\n", argv[1]);
	fprintf(fp,"\n%s (disassembled):\n\n", argv[1]);
	disassemble(file, memory, start, fp);
	free(memory);
	free(directive);
	return 0;
}
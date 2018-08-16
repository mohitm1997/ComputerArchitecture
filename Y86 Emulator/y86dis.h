#ifndef Y86DIS_
#define Y86DIS_

#include <stdio.h>

char* decimalToHex(int number);
int twosComplement(char* input);
char* hexToBinary(char* num);
char* getValue(int curr, int* memory);
char* getReg(int n);
void disassemble(FILE *file, int* memory, int start, FILE *fp);

#endif
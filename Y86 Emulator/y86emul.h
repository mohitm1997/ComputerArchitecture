#ifndef Y86EMUL_
#define Y86EMUL_

enum Status {AOK, HLT, INS, ADR};

typedef enum Status Status;

struct emulator
{
	int registers[8];
	Status currState;
	int size;
	int OF;
	int ZF;
	int SF;
	int ip;
};

typedef struct emulator emulator;

char* decimalToHex(int number);
double power(double base, int exponent);
int hexToDecimal(char* value);
int twosComplement(char* input);
char* hexToBinary(char* num);
char* bigEndianBinary(emulator* processor, int* memory);
char* stateToString(emulator* processor);
void flags(emulator *processor, int rB);
void execute(emulator* processor, int* memory);

#endif
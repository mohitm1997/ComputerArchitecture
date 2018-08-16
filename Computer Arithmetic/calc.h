// the enum Operation has values for valid operations, as well as INVALID to represent an invalid operation
enum Operation{ADDITION, SUBTRACTION, INVALID, MULTIPLICATION};

typedef enum Operation Operation;

// Number has an integer array for the input numbers, and the operation being performed
struct Number{
	int* numbers;
	Operation operation;
};

typedef struct Number Number;

// function prototypes:
Number* createNumber();
Operation getOperation (char * sign);
int getBase(char num);
int getLength(int num, int base);
int hexConv(char current);
char* reverse (char* string);
char* convertResult(int num, char base);
int convert(char* num);
int checkNum(char* num);
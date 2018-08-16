/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char * input;
};

typedef struct TokenizerT_ TokenizerT;

//An enum for the various states that each token can be in.
enum Type { NONE,DECIMAL,FLOATP,OCTAL,HEX,MAL,ZERO};

typedef enum Type Type;

/*
 * Global variables keeping track of the start of the token, the current index of the token (endpos),
 * the type the token currenyl is, and the individual token that is being returned.	
 */

int startpos;
int endpos;
Type currType;
char * returnToken;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	TokenizerT *T = (TokenizerT*)malloc(sizeof(TokenizerT));
	T -> input = ts;
	startpos = 0;
	endpos = 0;
	currType = NONE;
  return T;
}

/*
 * TokenReturn is called by each state function, when the token hits its end. It copies a single
 * token of the original input and puts it in 'returnToken' which is then returned.
 */
char *TokenReturn(TokenizerT * tk)
{
	returnToken = (char*)malloc(endpos - startpos + 1);
	strncpy(returnToken, &tk -> input[startpos], endpos - startpos);
	returnToken[endpos - startpos] = '\0';
	return returnToken;
}

/*
 * HexState is called when the first two characters are '0x', it will keep checking if
 * the characters in the token are hex digits, and will continue until it reaches the end of the token.
 */
char *HexState(TokenizerT * tk)
{
	if(isxdigit(tk -> input[endpos]))
	{
		currType = HEX;
		endpos++;
		return HexState(tk);
	}
	else
	{
		if(!isxdigit(tk -> input[endpos-1]))
		{
			endpos--;
		}
		return TokenReturn(tk);
	}
}

/*
 * OctalState is called when the first character is '0' and the second one is between 0 and 7, it will keep checking
 * if the characters in the token are octal, and will continue until it reaches the end of the token.
 */
char *OctalState(TokenizerT * tk)
{
	if(isdigit(tk -> input[endpos]) && tk -> input[endpos] >= '0' && tk -> input[endpos] <= '7' )
	{
		endpos++;
		return OctalState(tk);
	}
	else
	{
		return TokenReturn(tk);
	}
}

/*
 * FloatESignState is called when the token is a float which contains an 'E' and either a '+' or '-' and will keep checking
 * if the characters are digits until the end of the token is reached.
 */
char *FloatESignState(TokenizerT * tk)
{
	if(isdigit(tk -> input[endpos]))
	{
		endpos++;
		return FloatESignState(tk);
	}
	else
	{
		if(!isdigit(tk -> input[endpos - 1]))
		{
			endpos -= 2;
		}
		return TokenReturn(tk);
	}
}

/*
 * FloatEState is called when a decimal or float contains an 'E', it will change the state form decimal to floatp if necessary. 
 * It will keep checking if the characters are digits or a '+' or '-', calling itself when a digit and FloatESignState when there
 * is a sign. This will continue until FloatESignState is called or the token reaches its end.
 */
char *FloatEState(TokenizerT * tk)
{
	if(isdigit(tk -> input[endpos]))
	{
		currType = FLOATP;
		endpos++;
		return FloatEState(tk);
	}
	else if(tk -> input[endpos] == '+' || tk -> input[endpos] == '-')
	{
		currType = FLOATP;
		endpos++;
		return FloatESignState(tk);
	}
	else
	{
		if(!isdigit(tk -> input[endpos - 1]))
		{
			endpos--;
		}
		return TokenReturn(tk);
	}
}

/*
 * FloatState is called when the token contains a decimal, it will check if the characters are digits or 'E', if 'E' it will
 * call FloatEState. This will continue until the end of the token is reached or FloatEState is called. 
 */
char *FloatState(TokenizerT * tk)
{
	if(isdigit(tk -> input[endpos]))
	{
		endpos++;
		return FloatState(tk);	
	}
	else if((tk -> input[endpos] == 'E' || tk -> input[endpos] == 'e') && tk -> input[endpos - 1] != '.')
	{
		endpos++;
		return FloatEState(tk);
	}
	else
	{
		if(tk -> input[endpos - 1] == '.')
		{
			endpos--;
			if(tk -> input[endpos - 1] == '0')
			{
				currType = ZERO;
			}
			else
			{
				currType = DECIMAL;
			}
		}
		return TokenReturn(tk);
	}
}

/*
 * DecimalState is called when the token starts with an integer, it will keep checking if the characters in the token are digits,
 * or if there is a decimal point, or if there is an 'E'. If there is an 'E' FloatEState will be called, if there is a decimal point,
 * FloatState will be called. This will continue until either FloatState or FloatEState is called or if the end of the token is reached.
 */
char *DecimalState(TokenizerT * tk)
{
	if(isdigit(tk -> input[endpos]))
	{
		endpos++;
		return DecimalState(tk);
	}
	else if(tk -> input[endpos] == '.')
	{
		currType = FLOATP;
		endpos++;
		return FloatState(tk);
	}
	else if(tk -> input[endpos] == 'e' || tk -> input[endpos] == 'E')
	{
		endpos++;
		return FloatEState(tk);
	} 
	else
	{
		return TokenReturn(tk);
	}	
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free(tk);
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {
	startpos = endpos;

	while(isspace(tk -> input[endpos]))
	{
		startpos++;
		endpos++;
	}

	if(strlen(tk -> input) <= startpos)
	{
		return 0;
	}

	if(tk -> input[endpos] == '0')
	{
		endpos++;

		if(isspace(tk -> input[endpos]) || endpos >= strlen(tk -> input))
		{	
			returnToken = (char*)malloc(sizeof(char) + 1);
			currType = ZERO;
			strncpy(returnToken, &tk -> input[startpos], 1);
			returnToken[1] = '\0';
			return returnToken;
		}
		else if(tk -> input[endpos] == 'x' ||tk -> input[endpos] == 'X')
		{
			currType = ZERO;
			endpos++;
			return HexState(tk);	
		}
		else if(isdigit(tk -> input[endpos]) && tk -> input[endpos] >= '0' && tk -> input[endpos] <= '7')
		{
			currType = OCTAL;
			endpos++;
			return OctalState(tk);
		}
		else if(tk -> input[endpos] == '.')
		{
			currType = FLOATP;
			endpos++;
			return FloatState(tk);
		}
		else
		{
			currType = ZERO;
			return TokenReturn(tk);
		}
	}
	else if(isdigit(tk -> input[endpos]))
	{
		currType = DECIMAL;
		endpos++;
		return DecimalState(tk);
	}
	else
	{
		returnToken = (char*)malloc(sizeof(char) + 1);
		currType = MAL;
		strncpy(returnToken, &tk -> input[endpos], 1);
		returnToken[1] = '\0';
		endpos++;
		return returnToken;
	}
  return 0;
}

// getType will take a 'Type' as an argument and return the string form of the 'Type'.
const char * getType(Type t)
{
	switch(t)
	{
		case NONE: return "None";
		case DECIMAL: return "Decimal";
		case FLOATP: return "Float";
		case HEX: return "Hex";
		case OCTAL: return "Octal";
		case MAL: return "Malformed";
		case ZERO: return "Zero";
 	}
 	return NULL;
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	
	if(argc < 2)
	{
		printf("Error: too few arguments.\n");
		return 0;
	}
	else if(argc > 2)
	{
		printf("Error: too many arguments.\n");
		return 0;
	}

	TokenizerT *token = TKCreate(argv[1]);
	char * c;
	while(endpos < strlen(token -> input))
	{
		c = TKGetNextToken(token);
		if(c != 0)
		{
			if(currType == MAL)
			{
				printf("%-9s   [%#2x]\n", getType(currType), *c);
			}
			else
			{	
				printf("%-9s   %s\n", getType(currType), c);
			}
		}
		free(returnToken);
	}
	TKDestroy(token);
  return 0;
}
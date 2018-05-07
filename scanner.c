#include <iostream>
#include <list>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <string>
#include "token.h"
#include "scanner.h"

extern FILE *infp;

static int lineCount = 1;
static const int BUFFER = 80;
static char errmsg[BUFFER];
static char lookAhead;
 
/* FSA Table: 
   Rows correspond with states [0-19]
   Columns correspond with:
   {lowercase, UPPERCASE, Digits, DelimiterOperator, &, WS, EOF}
   Negative values correspond to error states
   Values >1000 correspond to final states
*/
const int FSATable[20][7] = {
	{   1,   -2,    9,   17,   18,    0, 1004},
	{   2,    2,    2, 1001, 1001, 1001, 1001},
	{   3,    3,    3, 1001, 1001, 1001, 1001},
	{   4,    4,    4, 1001, 1001, 1001, 1001},
	{   5,    5,    5, 1001, 1001, 1001, 1001},
	{   6,    6,    6, 1001, 1001, 1001, 1001},
	{   7,    7,    7, 1001, 1001, 1001, 1001},
	{   8,    8,    8, 1001, 1001, 1001, 1001},
	{  -1,   -1,   -1, 1001, 1001, 1001, 1001},
	{1002, 1002,   10, 1002, 1002, 1002, 1002},
	{1002, 1002,   11, 1002, 1002, 1002, 1002},
	{1002, 1002,   12, 1002, 1002, 1002, 1002},
	{1002, 1002,   13, 1002, 1002, 1002, 1002},
	{1002, 1002,   14, 1002, 1002, 1002, 1002},
	{1002, 1002,   15, 1002, 1002, 1002, 1002},
	{1002, 1002,   16, 1002, 1002, 1002, 1002},
	{1002, 1002,   -1, 1002, 1002, 1002, 1002},
	{1003, 1003, 1003, 1003, 1003, 1003, 1003},
	{  18,   18,   18,   18,   19,   18,   18},
	{   0,    0,    0,    0,    0,    0,    0}
}; 

/* scanner() scans characters from the input file, and compares their values
   along with the current state with the FSA table (FSATable, above), advancing
   through the table until either a final state or an error state is
   encountered. scanner() utilizes convertNextChar(), errMsg(), and makeToken()
   as helper functions to achieve this purpose. scanner() also tracks the
   number of lines in the given input file, adding this value to tokens
   produced by makeToken(). Sequential calls to scanner() recall the last
   scanned character using a lookAhead character.
*/
token scanner()
{
	int line = lineCount;
	int state = 0;
	int nextState = 0;
	char inputChar;
	int nextChar;
	token result;
	std::string str = "";
	std::string comment = "";
	
	/* Recall last lookAhead, if possible, otherwise scan for next character */
	if(lookAhead)
	{
		inputChar = lookAhead;
	}
	else
	{
		inputChar = fgetc(infp);
	}
	/* Convert most recently scanned character to FSA column value */
	nextChar = convertNextChar(inputChar);
	/* Increment the line counter if a newline character is found */
	if(inputChar == 10)
		lineCount++;
	
	while(state < 1000)
	{		
		
		nextState = FSATable[state][nextChar];
		/*std::cout<<"state = "<<state<<" nextState = "<<nextState<<" inputChar = "<<inputChar<<std::endl;*/
		
		/* Check for comment start*/
		if(nextState == 18)
		{
			/* Advance the through the FSA table*/
			state = nextState;
			/* Add the most recently scanned character to the token string */
			comment += inputChar;
			/* Scan the next character of the input file */
			inputChar = fgetc(infp);
			/* Convert most recently scanned character to FSA column value */
			nextChar = convertNextChar(inputChar);
			/* Increment the line counter if a newline character is found */
			if(inputChar == 10)
			{
				lineCount++;
				line = lineCount;
			}
		}	

		/* Check for comment end */
		else if(nextState == 19)
		{
			/* Advance the through the FSA table*/
			state = nextState;
			/* Add the most recently scanned character to the token string */
			comment += inputChar;
			/* Scan the next character of the input file */
			inputChar = fgetc(infp);
			/* Convert most recently scanned character to FSA column value */
			nextChar = convertNextChar(inputChar);
			/* Increment the line counter if a newline character is found */
			if(inputChar == 10)
			{
				lineCount++;
				line = lineCount;
			}
		}			
		
		/* Check for error state */
		else if(nextState < 0)
		{
			errMsg(nextState, line);
			exit(1);
		}
		
		/* Check for final state */
		else if(nextState >= 1000)
		{
			result = makeToken(nextState, str);
			state = nextState;
		}
		/* If not final state*/
		else
		{
			/* Advance the through the FSA table*/
			state = nextState;
			/* Add the most recently scanned character to the token string */
			str += inputChar;
			/* Remove white space from token strings */
			if(inputChar == 32 || inputChar == 9 || inputChar == 10)
				str = str.substr(0, str.size()-1);
			/* Scan the next character of the input file */
			inputChar = fgetc(infp);
			/* Convert most recently scanned character to FSA column value */
			nextChar = convertNextChar(inputChar);
			/* Increment the line counter if a newline character is found */
			if(inputChar == 10)
				lineCount++;
		}
	}
	
	/* Store the next scanned character for future scanner() calls */
	lookAhead = inputChar;
	/* Update the result token with the line number that it was created on */
	result.line = line;
	return result;
}


/* convertNextChar() takes a given character, and converts it into an integer
   value corresponding to:
   {lowercase, UPPERCASE, Digits, DelimiterOperator, &, WS, EOF}
   The returned value is to be used to determine which column to check next
   in the FSA table.
*/
int convertNextChar(char inputChar)
{
	int result = -1;
	/* Lowercase Letters */
	if(inputChar >= 97 && inputChar <= 122)
		result = 0;
	/* Uppercase Letters */
	if(inputChar >= 65 && inputChar <= 90)
		result = 1;
	/* Digits */
	if(inputChar >= 48 && inputChar <= 57)
		result = 2;
	/* Delimiters and Operators */
	if(inputChar == 37 || inputChar == 91 || inputChar == 93
	|| inputChar == 123 || inputChar == 125
	|| (inputChar >= 40 && inputChar <= 47)
	|| (inputChar >= 58 && inputChar <= 62))
		result = 3;
	/* &Comments& */
	if(inputChar == 38)
		result = 4;
	/* White Space */
	if(inputChar == 32 || inputChar == 9 || inputChar == 10)
		result = 5;
	/* End of File */
	if(inputChar == EOF)
		result = 6;
	/* Validate result */
	if(result < 0)
	{
		errMsg(-3, lineCount);
		exit(1);
	}
	return result;
}

/* errMsg() displays an error message in the event of an error during
   scanner(), complete with what type of error, and in which line it occurred. 
*/
void errMsg(int nextState, int line)
{
	if(nextState == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "Line %d: Token length too long.", line);
		perror(errmsg);	
	}
	if(nextState == -2)
	{
		snprintf(errmsg, sizeof(errmsg), "Line %d: Token may not start with uppercase letter.", line);
		perror(errmsg);	
	}
	if(nextState == -3)
	{
		snprintf(errmsg, sizeof(errmsg), "Line %d: Invalid input.", line);
		perror(errmsg);	
	}
}

/* makeToken() finalizes a token when scanner() reaches a final state in the
   FSA table [1001-1004]. makeToken() takes the state and string values from
   scanner(), determines what token type and token name the final state
   yields, and assigns those values to a token struct before returning the
   token to scanner().
*/
token makeToken(int nextState, std::string str)
{
	int i;
	token result;
	result.str = str;
	
	/* Identifier or Keyword */
	if(nextState == 1001)
	{
		for(i = 0; i < 12; i++)
		{
			/* Keyword */
			if(str.compare(keywords[i]) == 0)
			{
				result.id = (tokenType)(i+2);
				result.name = tokenNames[result.id];
				return result;
			}
		}
		/* Identifier */
		result.id = (tokenType)1;
		result.name = tokenNames[result.id];
		return result;
	}
	
	/* Integer */
	if(nextState == 1002)
	{
		result.id = (tokenType)0;
		result.name = tokenNames[result.id];
		return result;
	}
	
	/* Delimiter or Operator */
	if(nextState == 1003)
	{
		for(i = 0; i < 18; i++)
		{
			if(str.compare(delOps[i]) == 0)
			{
				result.id = (tokenType)(i+14);
				result.name = tokenNames[result.id];
				return result;
			}
		}
	}
	/* End of File */
	if(nextState == 1004)
	{
		result.id = (tokenType)32;
		result.name = tokenNames[result.id];
		return result;
	}
}
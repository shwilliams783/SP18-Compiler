#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

/* Function prototypes */
token scanner();
int convertNextChar(char);
void errMsg(int, int);
token makeToken(int, std::string);

#endif
#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "node.h"
#include "token.h"

extern std::string varStack[101];
extern int tos;
//extern FILE *outfp;
extern std::ofstream output;


/* Prototypes */
void push(std::string);
void pop();
int find(std::string);
void semError(int);
void firstPass(node_t*);

#endif
#ifndef STATSEM_H
#define STATSEM_H

#include "node.h"
#include "token.h"

extern std::string varStack[101];
extern int tos;


/* Prototypes */
void push(std::string);
void pop();
int find(std::string);
void statSemError(int);
void traversePostorder(node_t*);

#endif
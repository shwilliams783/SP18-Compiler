#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"

static std::string tokNames[33] = {"IntTK", "IDTK", "stopTK", "iterTK", "voidTK", "varTK", "returnTK",
"startTK", "readTK", "programTK", "printTK", "iffTK", "thenTK", "letTK",  "=TK",
">TK",	"<TK",	":TK",	"+TK",	"-TK",	"*TK",
"/TK",	"\%TK",	".TK", "(TK", ")TK", ",TK",
"{TK", "}TK", ";TK", "[TK", "]TK", "EOFTK"};

/* Function prototypes */
node_t* parser();
node_t* program();
node_t* block();
node_t* vars();
node_t* mvars();
node_t* expr();
node_t* M();
node_t* R();
node_t* stats();
node_t* mstat();
node_t* stat();
node_t* in();
node_t* out();
node_t* ifFunc();
node_t* loop();
node_t* assign();
node_t* RO();
node_t* genNode(int, int);
void parseError(int, int);

#endif
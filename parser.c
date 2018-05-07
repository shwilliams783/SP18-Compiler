#include <iostream>
#include <list>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <string>
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"

extern FILE *infp;
extern token tk;
static int currentDepth = -1;

static const int BUFFER = 80;
static char errmsg[BUFFER];

/* parser() creates a node that initializes the parser and tree builder with a
   root node of depth 0. */
node_t* parser()
{
	node_t* root = new node_t;
	root->depth = currentDepth;
	
	tk = scanner();
	root = program();
	if(tk.id == EOFTK)
	{
		currentDepth--;
		return root;
	}
	else
		parseError(32, tk.line);
}

/* program() creates a node that recognizes input file as a program with the
   keyword "Program" and calls vars() and block().*/
node_t* program()
{
	currentDepth++;
	node_t* p = genNode(progND, currentDepth);
	
	if(tk.id == progTK)
	{
		tk = scanner();
		p->child0 = vars();
		p->child1 = block();
		currentDepth--;
		return p;
	}
	else
		parseError(9, tk.line);
}

/* block() creates a node that recognizes blocks of code, beginning with
   "start" and ending with "stop". Within the block, vars() and stats()
   are called. */
node_t* block()
{
	currentDepth++;
	node_t* p = genNode(blocND, currentDepth);
	
	if(tk.id == starTK)
	{
		tk = scanner();
		p->child0 = vars();
		p->child1 = stats();
		if(tk.id == stopTK)
		{
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
			parseError(2, tk.line);
	}
	else
		parseError(7, tk.line);
}


/* vars() creates a node that is either empty, or initializes an Identifier to
   a given Integer value, both of which are stored as tokens. After
   initialization, mvars() is called. */
node_t* vars()
{
	currentDepth++;
	node_t* p = genNode(varsND, currentDepth);
	
	if(tk.id == varTK)
	{
		tk = scanner();
		if(tk.id == IDTK)
		{
			p->tokens[0] = tk;
			tk = scanner();
			if(tk.id == eqTK)
			{
				tk = scanner();
				if(tk.id == IntTK)
				{
					p->tokens[1] = tk;
					tk = scanner();
					p->child0 = mvars();
					currentDepth--;
					return p;
				}
				else
					parseError(0, tk.line);
			}
			else
				parseError(14, tk.line);
		}
		else
			parseError(1, tk.line);
	}
	else
		return p;
}

/* mvars() creates a node that is either empty, after recognizing a period
   token, or recognizes a colon token, then creates an Identifier, which is
   stored as a token. After initialization, mvars() is called again. */
node_t* mvars()
{
	currentDepth++;
	node_t* p = genNode(mvarND, currentDepth);
	
	if(tk.id == pdTK)
	{
		tk = scanner();
		currentDepth--;
		return p;		
	}
	else if(tk.id = colTK)
	{
		tk = scanner();
		if(tk.id == IDTK)
		{
			p->tokens[0] = tk;
			tk = scanner();
			p->child0 = mvars();
			currentDepth--;
			return p;
		}
		else
			parseError(1, tk.line);
	}
	else
		parseError(17, tk.line);
}

/* expr() creates a node that either contains an arithmetic operation (+-/*)
   between an M() node and another expr() node, or converts directly to an M()
   node. Arithmetic operations convert the node type from expr to either add, 
   sub, div, or mult.*/
node_t* expr()
{
	currentDepth++;
	node_t* p = genNode(exprND, currentDepth);
	
	std::cout<<"Entered expr(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	p->child0 = M();
	if(tk.id == plTK)
	{
		p->type = addND;
		tk = scanner();
		p->child1 = expr();
		currentDepth--;
		return p;
	}
	else if(tk.id == miTK)
	{
		p->type = subND;
		tk = scanner();
		p->child1 = expr();
		currentDepth--;
		return p;
	}
	else if(tk.id == fsTK)
	{
		p->type = divND;
		tk = scanner();
		p->child1 = expr();
		currentDepth--;
		return p;
	}
	else if(tk.id == stTK)
	{
		p->type = multND;
		tk = scanner();
		p->child1 = expr();
		currentDepth--;
		return p;
	}
	else
	{
		currentDepth--;
		return p;
	}
}

/* M() creates a node that either contains a modulus "%" token and another M()
   node, or converts to an R() node. */
node_t* M()
{
	currentDepth++;
	node_t* p = genNode(MND, currentDepth);
	
	std::cout<<"Entered M(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	if(tk.id == modTK)
	{
		p->tokens[0].id = modTK;
		tk = scanner();
		p->child0 = M();
		currentDepth--;
		return p;
	}
	else
	{
		p->child0 = R();
		currentDepth--;
		return p;
	}
}

/* R() creates a node that either contains an expr() node surrounded by
   parenthesis, or stores an identifier token, or stores an integer token*/
node_t* R()
{
	currentDepth++;
	node_t* p = genNode(RND, currentDepth);
	
	std::cout<<"Entered R(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	if(tk.id == lpTK)
	{
		tk = scanner();
		p->child0 = expr();
		if(tk.id == rpTK)
		{
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
			parseError(25, tk.line);
	}
	else if(tk.id == IDTK)
	{
		p->tokens[0] = tk;
		tk = scanner();
		currentDepth--;
		return p;
	}
	else if(tk.id == IntTK)
	{
		p->tokens[0] = tk;
		tk = scanner();
		currentDepth--;
		return p;
	}
	else
		parseError(0, tk.line);
}

/* stats() creates a node that creates a stat() node and mstat() node. */
node_t* stats()
{
	currentDepth++;
	node_t* p = genNode(stsND, currentDepth);
	
	std::cout<<"Entered stats(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	p->child0 = stat();
	p->child1 = mstat();
	currentDepth--;
	return p;
}

/* mstat() creates either an empty node, or a node that creates a stat() node
   and another mstat() node. The stat() node requires the next token to be
   any of the following: {readTK, prntTK, starTK, iffTK, iterTK, letTK}.*/
node_t* mstat()
{
	currentDepth++;
	node_t* p = genNode(mstND, currentDepth);
	std::cout<<"Entered mstat(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	if(tk.id == readTK || tk.id == prntTK || tk.id == starTK || tk.id == iffTK || tk.id == iterTK || tk.id == letTK)
	{
		p->child0 = stat();
		p->child1 = mstat();
		currentDepth--;
		return p;
	}
	else
	{
		currentDepth--;
		return p;
	}
}

/* stat() creates a node that requires the next token to be any of the
   following: {readTK, prntTK, starTK, iffTK, iterTK, letTK}. stat() creates a
   corresponding new node to handle the given token:
   {in(), out(), block(), ifFunc(), loop(), assign()}.   */
node_t* stat()
{
	currentDepth++;
	node_t* p = genNode(statND, currentDepth);
	
	std::cout<<"Entered stat(), current token = "<<tk.name<<std::endl; // REMOVE AFTER DEBUGGING
	
	// <in>
	if(tk.id == readTK)
	{
		tk = scanner();
		p->child0 = in();
		currentDepth--;
		return p;
	}
	// <out>
	else if(tk.id == prntTK)
	{
		tk = scanner();
		p->child0 = out();
		currentDepth--;
		return p;
	}
	// <block>
	else if(tk.id == starTK)
	{
		p->child0 = block();
		currentDepth--;
		return p;
	}
	// <if>
	else if(tk.id == iffTK)
	{
		tk = scanner();
		p->child0 = ifFunc();
		currentDepth--;
		return p;
	}
	// <loop>
	else if(tk.id == iterTK)
	{
		tk = scanner();
		p->child0 = loop();
		currentDepth--;
		return p;
	}
	// <assign>
	else if(tk.id == letTK)
	{
		tk = scanner();
		p->child0 = assign();
		currentDepth--;
		return p;
	}
	else
		parseError(13, tk.line);
	
}

/* in() creates a node that stores an identifier token.*/
node_t* in()
{
	currentDepth++;
	node_t* p = genNode(inND, currentDepth);
	
	if(tk.id == IDTK)
	{
		p->tokens[0] = tk;
		tk = scanner();
		if(tk.id == pdTK)
		{
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
			parseError(23, tk.line);
	}
	else
		parseError(1, tk.line);
}

/* out() creates a node that creates an expr() node.*/
node_t* out()
{
	currentDepth++;
	node_t* p = genNode(outND, currentDepth);
	
	p->child0 = expr();
	if(tk.id == pdTK)
	{
		tk = scanner();
		currentDepth--;
		return p;
	}
	else
		parseError(23, tk.line);
}

/* ifFunc() creates a node that creates 3 nodes contained within parenthesis,
   (expr(), RO(), expr()), followed by a fourth node, stat(). */
node_t* ifFunc()
{
	currentDepth++;
	node_t* p = genNode(iffND, currentDepth);
	
	if(tk.id == lpTK)
	{
		tk = scanner();
		p->child0 = expr();
		p->child1 = RO();
		p->child2 = expr();
		if(tk.id == rpTK)
		{
			tk = scanner();
			p->child3 = stat();
			currentDepth--;
			return p;
		}
		else
			parseError(25, tk.line);
	}
	else
		parseError(24, tk.line);
}

/* loop() creates a node that creates 3 nodes contained within parenthesis,
   (expr(), RO(), expr()), followed by a fourth node, stat(). */
node_t* loop()
{
	currentDepth++;
	node_t* p = genNode(loopND, currentDepth);
	
	if(tk.id == lpTK)
	{
		tk = scanner();
		p->child0 = expr();
		p->child1 = RO();
		p->child2 = expr();
		if(tk.id == rpTK)
		{
			tk = scanner();
			p->child3 = stat();
			currentDepth--;
			return p;
		}
		else
			parseError(25, tk.line);
	}
	else
		parseError(24, tk.line);
}

/* assign() creates a node that stores an identifer, checks for an equal
   "=" token, then creates an expr() node.*/
node_t* assign()
{
	currentDepth++;
	node_t* p = genNode(asgnND, currentDepth);
	
	if(tk.id == IDTK)
	{
		p->tokens[0] = tk;
		tk = scanner();
		if(tk.id == eqTK)
		{
			tk = scanner();
			p->child0 = expr();
			if(tk.id == pdTK)
			{
				tk = scanner();
				currentDepth--;
				return p;
			}
			else
				parseError(23, tk.line);
		}
		else
			parseError(14, tk.line);
	}
	else
		parseError(1, tk.line);
}

/* RO() creates a node that neither creates any further nodes, nor does it
   store any tokens. Instead, the RO() node changes its type to one of the
   following: {ltND, leND, gtND, geND, eqND, eqeqND}, depending on the tokens
   that are scanned.*/
node_t* RO()
{
	currentDepth++;
	node_t* p = genNode(ROND, currentDepth);
	
	if(tk.id == ltTK)
	{
		p->type = ltND;
		tk = scanner();
		if(tk.id == ltTK)
		{
			p->type = leND;
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
		{
			currentDepth--;
			return p;
		}
	}
	else if(tk.id == gtTK)
	{
		p->type = gtND;
		tk = scanner();
		if(tk.id == gtTK)
		{
			p->type = geND;
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
		{
			currentDepth--;
			return p;
		}
	}
	else if(tk.id == eqTK)
	{
		p->type = eqND;
		tk = scanner();
		if(tk.id == eqTK)
		{
			p->type = eqeqND;
			tk = scanner();
			currentDepth--;
			return p;
		}
		else
		{
			currentDepth--;
			return p;
		}
	}
	else
		parseError(14, tk.line);
}

/* genNode() creates a new node with the given node type and node depth */
node_t* genNode(int type, int depth)
{
	node_t* node = new node_t;
	node->type = (nodeType)type;
	node->depth = depth;
	return node;
}

/* parseError() displays an error message containing the line number where the
   syntax error was found, and what token was expected */
void parseError(int errCode, int line)
{
	std::cerr<<"Error: line "<<line<<", expected "<<tokNames[errCode]<<" token."<<std::endl;
	exit(errCode);
}

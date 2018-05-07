#ifndef NODE_H
#define NODE_H

#include <list>
#include <string>
#include "token.h"

/* 	 nodeType  {0,	1,	2,	3,	4,	5,	6,	7, 
				8,	9,	10,	11,	12,	13,	14,	15,
				16,	17,	18,	19, 20, 21, 22, 23,
				24, 25};
*/

enum nodeType {progND,	blocND, varsND,	mvarND,	exprND,	MND,	RND,	stsND,
			   statND,	mstND,	inND,	outND,	iffND,	loopND,	asgnND,	ROND,
			   addND,	subND,	divND,	multND,	ltND,	leND,	gtND,	geND,
			   eqND,	eqeqND};

struct node_t{
	/* Constructor */
	node_t()
	{
		depth = 0;
	}
	
	int depth;
	nodeType type;
	node_t* child0;
	node_t* child1;
	node_t* child2;
	node_t* child3;
	token tokens[4];
};

#endif
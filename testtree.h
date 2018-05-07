#ifndef TESTTREE_H
#define TESTTREE_H

#include "node.h"

/* 	 nodeType  					   {0,	1,	2,	3,
									4,	5,	6,	7, 8,
									9,	10,	11,	12,
									13,	14,	15, 16,
									17,	18,	19, 20, 21,
									22, 23,	24, 25};
*/

static std::string nodeNames[26] = {"<program>", "<block>", "<vars>", "<mvars>",
									"<expr>", "<M>", "<R>", "<stats>", "<stat>",
									"<mStat>", "<in>", "<out>", "<if>",
									"<loop>", "<assign>", "<RO>", "<add>",
									"<sub>", "<div>", "<mult>",	"<lt>", "<le>",
									"<gt>", "<ge>", "<eq>",	"<eqeq>"};

/* Prototypes */
void traversePreorder(node_t*);

#endif
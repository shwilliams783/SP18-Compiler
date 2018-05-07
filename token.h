#ifndef TOKEN_H
#define TOKEN_H

#include <list>
#include <string>

/* 	 tokenType {0,	1,	2,	3,	4,	5,	6,	7, 
				8,	9,	10,	11,	12,	13,	14,	15,
				16,	17,	18,	19,	20,	21,	22,	23,
				24,	25,	26,	27,	28,	29,	30,	31,
				32};
*/

enum tokenType {IntTK,	IDTK,	stopTK,	iterTK,	voidTK,	varTK,	retTK,	starTK,
				readTK,	progTK,	prntTK,	iffTK,	thenTK, letTK,	eqTK,	gtTK,
				ltTK,	colTK,	plTK,	miTK,	stTK,	fsTK,	modTK,	pdTK,
				lpTK,	rpTK,	coTK,	lcbTK,	rcbTK,	semTK,	lbrTK,	rbrTK,
				EOFTK};

static std::string tokenNames[33] = {"IntTK", "IDTK", "stopTK", "iterTK", "voidTK", "varTK", "returnTK",
"startTK", "readTK", "programTK", "printTK", "iffTK", "thenTK", "letTK",  "=TK",
">TK",	"<TK",	":TK",	"+TK",	"-TK",	"*TK",
"/TK",	"\%TK",	".TK", "(TK", ")TK", ",TK",
"{TK", "}TK", ";TK", "[TK", "]TK", "EOFTK"};

static std::string keywords[12] = {"stop", "iter", "void", "var", "return",
									"start", "read", "program", "print", 
									"iff", "then", "let"};
									
static std::string delOps[18] = {"=", ">", "<", ":", "+", "-", "*", "/", 
									"\%", ".", "(", ")", ",", "{", "}", ";",
									"[", "]"};

struct token{	
	token()
	{
		id = IntTK;
		line = 1;
		str = "NULL";
	}

	tokenType id;
	std::string name;
	std::string str;
	int line;
};

#endif
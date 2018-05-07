#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"
#include "testtree.h"
#include "semantics.h"

static const int BUFFER = 80; 
FILE *infp;
//FILE *outfp;
std::ofstream output;
token tk;

void inputValidation(int, char**);

/* main() validates user command line input for selecting input files to be
   scanned into tokens by parser(), which will produce a parse tree for the
   back end. After parser() makes the parse tree, traversePreorder() prints
   the node types and any stored tokens, indented 2 spaces per depth in the
   tree.
*/
int main(int argc, char* argv[])
{
	node_t* root = new node_t;
	/* Validates command line arguments */
	inputValidation(argc, argv);
	/* Initializes the parser */
	root = parser();
	/* Prints the Parse Tree (DEBUGGING) */
	//traversePreorder(root);
	/* Validates Static Semantics */
	firstPass(root);
	std::cout<<"Static semantics complete."<<std::endl;
}

/* inputValidation() validates command line arguments and sets the file pointer
   to the target input file. The command line can take a .sp18 file without the
   extension:
   ./scanner FILENAME
   or with the extension
   ./scanner < FILENAME.sp18
*/
void inputValidation(int argc, char** argv)
{
	char outputName[BUFFER] = "";
	
	/* default to stdin */
	infp = stdin;
	
	/* read from a file listed on the command line */
	if(argc == 2)
	{
		if(infp = fopen(strcat(argv[1],".sp18"), "r"))
		{
			strcat(outputName, argv[1]);
			outputName[strlen(outputName)-5] = '\0';
			strcat(outputName, ".asm");
			output.open(outputName);
		}
		else
		{
			perror("Invalid file name");
			exit(1);
		}
	}
	else
	{
		output.open("out.asm");
	}
}

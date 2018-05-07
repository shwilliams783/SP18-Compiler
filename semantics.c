#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <string>
#include "semantics.h"


std::string varStack[101]; // Variable Stack
int tos = 0; // Top of Stack
static std::string tempVar[101]; // Temporary Variables
static int tempCount = 0;
//static std::string label[101]; // Labels
static int labelCount = 0;
static int varCount = 0;
static const int STACKMAX = 99; // varStack holds strings at indices 0-99
static const int BUFFER = 80; // Maximum char[] buffer size

/* push() adds the given string to varStack, and monitors the capacity of 
   varStack, generating a semError() if tos >= STACKMAX. (varStack holds
   100 srtings maximum). A successful push() will also increment tos. */
void push(std::string str)
{
	// Check varStack capacity, add if under capacity
	if(tos < STACKMAX)
	{
		varStack[tos] = str;
		output <<"PUSH "<<std::endl;
		tos++;		
	}
	// Stack Overflow Error
	else
	{
		semError(1);
	}
}

/* pop() "removes" strings from varStack (by setting them to ""). A successful
   pop() will also decrement tos. */
void pop()
{
	// Remove string from tos
	varStack[tos] = "";
	if(tos > 0)
	{
		output <<"POP "<<std::endl;
		// Decrement tos
		tos--;
	}
}

/* find() returns -1 in the event that the given string, str, is not contained
   within varStack. If the string is found within varStack, its location
   relative to tos will be returned. find() works globally, so it's result is
   tested against varCount when testing for local variable allocation.  */
int find(std::string str)
{
	// Default to -1, (not found)
	int result = -1;
	int i;
	for(i = tos; i >= 0; i--)
	{
		// If the provided string matches the ith value of varStack
		if(str.compare(varStack[i]) == 0)
		{
			// Set result to its location relative to tos, and break the loop
			result = tos - i;
			break;
		}
	}
	return result;
}

/* semError() takes a given error code, displays an error message to cerr
   describing the error, and exits the program. */
void semError(int errCode)
{
	switch(errCode)
	{
		case 1:
			// Stack overflow
			std::cerr<<"Error: Static semantics: varStack overflow."<<std::endl;
			exit(errCode);
		case 2:
			// Duplicate variable allocation in same scope
			std::cerr<<"Error: Static semantics: Duplicate variable found in scope."<<std::endl;
			exit(errCode);
		case 3:
			// Undleclares variable
			std::cerr<<"Error: Static semantics: Undeclared variable."<<std::endl;
			exit(errCode);
		default:
			// Unknown other error
			std::cerr<<"Error: Semantics"<<std::endl;
			exit(errCode);
	}
}

/*	firstPass() outputs the node types and tokens contained in each node of the tree
	in the order: root (self), child0, child1, child2, child3.*/
void firstPass(node_t* root)
{
	int i;
	
	// Preserve previous varCount 
	int previousVarCount = varCount;
	
	// Resolve children, left to right
	
	// Variable Scope: <block> open
	if(root->type == blocND)
	{
		varCount = 0;
	}
	
	// Variable Allocation: <vars>/<mvars> open
	if(root->type == varsND || root->type == mvarND)
	{
		// Check if node contains an identifier
		if(root->tokens[0].str.compare("NULL") != 0)
		{	
			// Check to see if the Identifier is already defined in this scope
			if(find(root->tokens[0].str) <= varCount && find(root->tokens[0].str) >= 0)
			{
				semError(2);
			}
			// Valid variable allocation: add to stack
			else
			{
				push(root->tokens[0].str);
				varCount++;
			}
		}
	}
	
	// Variable Usage (read access): <R> open
	if(root->type == RND)
	{
		// Store find once, rather than repeating the search - more optimized
		int found = find(root->tokens[0].str);
		
		// IDTK found locally
		if(found <= varCount && found >= 0)
		{
			output<<"STACKR "<<found<<std::endl;
		}

		// IDTK found globally
		else if(found >= 0)
		{
			//output<<"STACKR "<<found<<std::endl;
		}
		
		// <R> contains int or nothing -> (<expr>)
		else if(root->tokens[0].id == IntTK || root->tokens[0].str.compare("NULL") == 0);
		
		// Undeclared variable usage
		else
			semError(3);
	}
	
	// Variable Usage (write access): <assign> open
	if(root->type == asgnND)
	{
		// Store find once, rather than repeating the search - more optimized
		int found = find(root->tokens[0].str);
		
		// IDTK found locally
		if(found <= varCount && found >= 0)
		{
			output<<"STACKW "<<found<<std::endl;
		}

		// IDTK found globally
		else if(found >= 0)
		{
			//output<<"STACKW "<<found<<std::endl;
		}
		
		// <R> contains int or nothing -> (<expr>)
		else if(root->tokens[0].id == IntTK || root->tokens[0].str.compare("NULL") == 0);
		
		// Undeclared variable usage
		else
			semError(3);
	}
	
	// Variable Usage (write access): <in> open
	if(root->type == inND)
	{
		// Store find once, rather than repeating the search - more optimized
		int found = find(root->tokens[0].str);
		std::cout<<"Variable = "<<root->tokens[0].str<<", found = "<<found<<", varCount = "<<varCount<<", tos = "<<tos<<std::endl; //REMOVE AFTER DEBUGGING
		
		// IDTK found locally
		if(found <= varCount && found >= 0)
		{
			std::cout<<"Local Variable found!"<<std::endl; //REMOVE AFTER DEBUGGING
			// Create new temp
			int tempNo = tempCount;
			std::stringstream ss;
			tempCount++;
			ss<<"T";
			ss<<tempNo;
			tempVar[tempNo] = ss.str();
			push(ss.str());
			varCount++;
			// READ T#
			output<<"READ "<<ss.str()<<std::endl;
			// LOAD T#
			output<<"LOAD "<<ss.str()<<std::endl;
			// STACKW found
			output<<"STACKW "<<found<<std::endl;
		}

		// IDTK found globally
		else if(found >= 0)
		{
			std::cout<<"Global Variable found!"<<std::endl; //REMOVE AFTER DEBUGGING
			output<<"READ "<<root->tokens[0].str<<std::endl;
		}
		
		// <R> contains int or nothing -> (<expr>)
		else if(root->tokens[0].id == IntTK || root->tokens[0].str.compare("NULL") == 0);
		
		// Undeclared variable usage
		else
			semError(3);
	}
	
	/***************************End Static Semantics**************************/
	
	// <expr>-><add>
	if(root->type == addND)
	{
		// Call right child
		firstPass(root->child1);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call left child
		firstPass(root->child0);
		// ADD T#		
		output<<"ADD "<<ss.str()<<std::endl;
		//std::cout<<tempVar[tempNo]<<std::endl; // REMOVE AFTER DEBUGGING
	}
	
	// <expr>-><sub>
	if(root->type == subND)
	{
		// Call right child
		firstPass(root->child1);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call left child
		firstPass(root->child0);
		// ADD T#		
		output<<"SUB "<<ss.str()<<std::endl;
		//std::cout<<tempVar[tempNo]<<std::endl; // REMOVE AFTER DEBUGGING
	}
	
	// <expr>-><div>
	if(root->type == divND)
	{
		// Call right child
		firstPass(root->child1);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call left child
		firstPass(root->child0);
		// ADD T#		
		output<<"DIV "<<ss.str()<<std::endl;
		//std::cout<<tempVar[tempNo]<<std::endl; // REMOVE AFTER DEBUGGING
	}
	
	// <expr>-><mult>
	if(root->type == multND)
	{
		// Call right child
		firstPass(root->child1);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call left child
		firstPass(root->child0);
		// ADD T#		
		output<<"MULT "<<ss.str()<<std::endl;
		//std::cout<<tempVar[tempNo]<<std::endl; // REMOVE AFTER DEBUGGING
	}
	
	// <M>
	if(root->type == MND)
	{
		//std::cout<<"MND node found."<<std::endl; // REMOVE AFTER DEBUGGING
		if(root->tokens[0].id == modTK)
		{	
			//std::cout<<"\t\% token found."<<std::endl; // REMOVE AFTER DEBUGGING
			firstPass(root->child0);
			//std::cout<<"\t<M> firstPass(child0) returned!"<<std::endl; // REMOVE AFTER DEBUGGING
			output<<"MULT -1"<<std::endl;
		}
		else
		{
			firstPass(root->child0);
		}
	}
	
	// <assign>
	if(root->type == asgnND)
	{
		// Call first child
		firstPass(root->child0);
		// Store ACC in variable
		output<<"STORE "<<root->tokens[0].str<<std::endl;
	}
	
	// <if>
	if(root->type == iffND)
	{
		// Call third child
		firstPass(root->child2);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call first child
		firstPass(root->child0);
		// Subtract from T#
		output<<"SUB "<<ss.str()<<std::endl;
		// Create new label
		int labelNo = labelCount;
		std::stringstream ll;
		labelCount++;
		ll<<"L";
		ll<<labelNo;
		// Print BR statement
		// Less than (<)
		if(root->child1->type == ltND)
		{
			output<<"BRZPOS "<<ll.str()<<std::endl;
		}
		// Less than or equal to (<=)
		if(root->child1->type == leND)
		{
			output<<"BRPOS "<<ll.str()<<std::endl;
		}
		// Greater than (>)
		if(root->child1->type == gtND)
		{
			output<<"BRZNEG "<<ll.str()<<std::endl;
		}
		// Greater than or equal to (>=)
		if(root->child1->type == geND)
		{
			output<<"BRNEG "<<ll.str()<<std::endl;
		}
		// Equal to (==)
		if(root->child1->type == eqeqND)
		{
			output<<"BRZERO "<<ll.str()<<std::endl;
		}
		// Not equal to (=)
		if(root->child1->type == eqND)
		{
			output<<"BRNEG "<<ll.str()<<std::endl;
			output<<"BRPOS "<<ll.str()<<std::endl;
		}
		// Call fourth child
		firstPass(root->child3);
		// Print L#
		output<<ll.str()<<": NOOP"<<std::endl;		
	}
	
	// <iter>
	if(root->type == loopND)
	{
		// Create new label for out loop
		int labelNoIn = labelCount;
		std::stringstream ll;
		labelCount++;
		ll<<"L";
		ll<<labelNoIn;
		// Create new label for in loop
		int labelNoOut = labelCount;
		std::stringstream tt;
		labelCount++;
		tt<<"L";
		tt<<labelNoOut;
		// Print L# for in loop
		output<<tt.str()<<": NOOP"<<std::endl;		
		// Call third child
		firstPass(root->child2);
		// Create new temp
		int tempNo = tempCount;
		std::stringstream ss;
		tempCount++;
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		// STORE T#
		output<<"STORE "<<ss.str()<<std::endl;
		// Call first child
		firstPass(root->child0);
		// Subtract from T#
		output<<"SUB "<<ss.str()<<std::endl;
		// Print BR statement for out
		// Less than (<)
		if(root->child1->type == ltND)
		{
			output<<"BRZPOS "<<ll.str()<<std::endl;
		}
		// Less than or equal to (<=)
		if(root->child1->type == leND)
		{
			output<<"BRPOS "<<ll.str()<<std::endl;
		}
		// Greater than (>)
		if(root->child1->type == gtND)
		{
			output<<"BRZNEG "<<ll.str()<<std::endl;
		}
		// Greater than or equal to (>=)
		if(root->child1->type == geND)
		{
			output<<"BRNEG "<<ll.str()<<std::endl;
		}
		// Equal to (==)
		if(root->child1->type == eqeqND)
		{
			output<<"BRZERO "<<ll.str()<<std::endl;
		}
		// Not equal to (=)
		if(root->child1->type == eqND)
		{
			output<<"BRNEG "<<ll.str()<<std::endl;
			output<<"BRPOS "<<ll.str()<<std::endl;
		}
		// Call fourth child
		firstPass(root->child3);
		// BR back to in loop
		output<<"BR "<<tt.str()<<std::endl;
		// Print L# for out loop
		output<<ll.str()<<": NOOP"<<std::endl;		
	}
	
	
	/*********************************Children********************************/
	
	if(root->type != addND && root->type != subND && root->type != divND
	&& root->type != multND && root->type != MND && root->type != asgnND
	&& root->type != iffND)
	{
		/* Resolve child0 */
		if(root->child0 != NULL)
		{
			firstPass(root->child0);
		}
		/* Resolve child1 */
		if(root->child1 != NULL)
		{
			firstPass(root->child1);
		}
		/* Resolve child2 */
		if(root->child2 != NULL)
		{
			firstPass(root->child2);
		}
		/* Resolve child3 */
		if(root->child3 != NULL)
		{
			firstPass(root->child3);
		}
	}	
	/**********************************Roots**********************************/
	
	// Scope Closure: <print>
	if(root->type == outND)
	{
		int tempNo = tempCount;
		std::stringstream ss;
		
		tempCount++;
		
		ss<<"T";
		ss<<tempNo;
		tempVar[tempNo] = ss.str();
		push(ss.str());
		varCount++;
		output<<"STORE "<<ss.str()<<std::endl;
		output<<"WRITE "<<ss.str()<<std::endl;
		//std::cout<<tempVar[tempNo]<<std::endl; // REMOVE AFTER DEBUGGING
	}
	
	// Scope Closure: <block>
	if(root->type == blocND)
	{
		// Pop() varCount variables off of the varStack
		for(i = 0; i <= varCount; i++)
			pop();

		// Reset varCount to previousVarCount before closing <block>
		varCount = previousVarCount;
	}
	
	// Scope Closure: <prog>
	if(root->type == progND)
	{
		output << "STOP\n";
		for(i = tempCount-1; i >= 0; i--)
		{
			output<<tempVar[i]<<" 0"<<std::endl;
		}
		for(i = tos; i >= 0; i--)
		{
			output<<varStack[i]<<" 0"<<std::endl;
		}
	}
}
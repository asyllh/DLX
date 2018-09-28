/*--------------/
ALH
constants.cpp
21/09/18
/--------------*/
#include "constants.h"
//#include <iostream>


const int MAX_LEVEL = 150; // at most this many rows in a solution
const int MAX_COLS = 10000; // at most this many columns
const int MAX_NODES = 1000000; // at most this many nonzero elements in the matrix

int solution = 0;
int highestLevel = 0;
std::vector<std::vector<std::vector<std::string> > > rowSolutions;
Node nodeArray[MAX_NODES]; //place for nodes
Node* choice[MAX_LEVEL]; //the row and column chosen on each level
Column colArray[MAX_COLS + 2]; //place for column records


void ProgramInfo(){
    std::cout << "Please provide a file.\n";
}
//Column root = colArray[0]; // First column struct, head of list of columns, has no name.


//const int MAX_DEGREE = 10000; // at most this many branches per search tree node
//int level; // number of choices in current partial solution
//int updates; // number of times we deleted a list element
//int verbose; // >0 to show solutions, >1 to show partial ones too
//int count = 0; // number of solutions found so far
//int maxb = 0; // maximum branching factor actually needed
//int maxl = 0; // maximum level actually reached
//int profile[MAX_LEVEL][MAX_DEGREE]; // tree nodes of given level and degree
//int updateProfile[MAX_LEVEL]; // updates at a given level
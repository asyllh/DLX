/*--------------/
ALH
constants.cpp
21/09/18
/--------------*/
#include "constants.h"
//#include <iostream>


const int maxLevel = 150;
const int maxDegree = 10000;
const int maxCols = 10000;
const int maxNodes = 1000000;
const int bufSize = 80000;

char buf[bufSize];
Node nodeArray[maxNodes]; //place for nodes
Node* choice[maxLevel]; //the row and column chosen on each level
Column colArray[maxCols + 2]; //place for column records
Column root = colArray[0];

//int level; // number of choices in current partial solution
//int updates; // number of times we deleted a list element
//int verbose; // >0 to show solutions, >1 to show partial ones too
//int count = 0; // number of solutions found so far
//int maxb = 0; // maximum branching factor actually needed
//int maxl = 0; // maximum level actually reached
//int profile[maxLevel][maxDegree]; // tree nodes of given level and degree
//int updateProfile[maxLevel]; // updates at a given level

/*--------------/
ALH
constants.cpp
21/09/18
/--------------*/
#include "constants.h"


int solution = 0;
int highestLevel = 0;
Column* colRoot = nullptr;
std::vector<Node*> choice;
std::vector<std::vector<std::vector<std::string> > > rowSolutions;
std::vector<std::vector<std::vector<Node*> > > rowSoln;

void ProgramInfo(){
    std::cout << "Please provide a file.\n";
}

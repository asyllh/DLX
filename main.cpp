/*--------------/
ALH
C++ program implementing DLX
main.cpp
01/09/18
/--------------*/
/** TO DO **/
/* Is the output using choice[level] a feasible solution? How do we check?
 * How do we label the columns/rows in EVOL, i.e. how to label strips A, B, C or 0, 1, 2,..
   and does it need to be a string or a char?
 * How do we convert output from EVOl to use in DLX?
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include "func.h"

int main (int argc, char** argv) {

    if(argc < 2){
        ProgramInfo();
        exit(1);
    }

    Timer timer;

    Column* currentCol = colArray + 1;
    std::ifstream ifs;
    ifs.open(argv[1]);
    if(!ifs){
        std::cerr << "Cannot open file." << std::endl;
        exit(1);
    }
    std::string columnInput;
    std::getline(ifs, columnInput);
    std::stringstream ss(columnInput);
    while(ss){
        std::string tempString;
        ss >> tempString;
        if(!ss){ break; }
        currentCol->name = tempString;
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        currentCol->prev = currentCol - 1;
        (currentCol - 1)->next = currentCol;
        ++currentCol;
    }
    (currentCol - 1)->next = &colArray[0]; //&root
    colArray[0].prev = currentCol - 1; //root.prev*/


    Node* currentNode = nodeArray;
    while (!ifs.eof()) {
        std::string rowInput;
        std::getline(ifs, rowInput);
        std::stringstream ss(rowInput);

        Node* rowStart = nullptr;
        Column* ccol = nullptr;

        while(!ss.eof()){
            std::string tempString;
            ss >> tempString;
            if(!ss) { break; }
            for(ccol = colArray; tempString.compare(ccol->name) != 0; ++ccol);
            if (!rowStart) {
                rowStart = currentNode;
            }
            else {
                currentNode->left = currentNode - 1, (currentNode - 1)->right = currentNode;
            }
            currentNode->col = ccol;
            currentNode->up = ccol->head.up, ccol->head.up->down = currentNode;
            ccol->head.up = currentNode, currentNode->down = &ccol->head;
            ++ccol->len;
            ++currentNode;
        }
        if (!rowStart) {
            std::cout << "Empty row." << std::endl;
            exit(1);
        }
        rowStart->left = currentNode - 1, (currentNode - 1)->right = rowStart;
    }

    ifs.close();

    currentNode = nullptr;
    currentCol = nullptr;
    Column* bestCol = nullptr;

    //Start of search(k) recursive procedure
    int level = 0;
    RecursiveSearch(level, currentNode, bestCol);


} //END INT MAIN


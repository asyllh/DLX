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
 * Change input so that it reads in 0-1 matrix rather than file with column names and rows only
   showing which column has a node in the row (so that it can use output matrix from EVOL)
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

    Column* currentCol;
    Column* prevCol = new Column;
    colRoot = prevCol;
    std::ifstream ifs;
    ifs.open(argv[1]);
    if(!ifs){
        std::cerr << "[ERROR]: Cannot open file." << std::endl;
        exit(1);
    }
    std::string columnInput;
    std::getline(ifs, columnInput);
    std::stringstream ss(columnInput);
    while(ss){
        std::string tempString;
        ss >> tempString;
        if(!ss){ break; }
        currentCol = new Column;
        currentCol->name = tempString;
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        currentCol->prev = prevCol;
        prevCol->next = currentCol;
        prevCol = currentCol;
    }
    prevCol->next = colRoot;
    colRoot->prev = prevCol;
    currentCol = nullptr;
    prevCol = nullptr;

    /*Column* currentCol = colArray + 1;
    std::ifstream ifs;
    ifs.open(argv[1]);
    if(!ifs){
        std::cerr << "[ERROR]: Cannot open file." << std::endl;
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
    (currentCol - 1)->next = &colArray[0];
    colArray[0].prev = currentCol - 1;*/

    Node* currentNode;
    while (!ifs.eof()) {
        std::string rowInput;
        std::getline(ifs, rowInput);
        if(rowInput.empty()){
            break;
        }
        std::stringstream ss(rowInput);
        Node* rowStart = nullptr;
        Column* ccol = nullptr;
        Node dummy;
        Node* prevNode = &dummy;
        //Node* prevNode = new Node;
        while(!ss.eof()){
            std::string tempString;
            ss >> tempString;
            if(!ss) { break; }
            currentNode = new Node;
            for(ccol = colRoot->next; tempString != ccol->name && ccol != colRoot; ccol = ccol->next);
            if (!rowStart) {
                rowStart = currentNode;
            }
            else {
                currentNode->left = prevNode;
                prevNode->right = currentNode;
            }
            currentNode->col = ccol;
            currentNode->up = ccol->head.up, ccol->head.up->down = currentNode;
            ccol->head.up = currentNode, currentNode->down = &ccol->head;
            ++ccol->len;
            prevNode = currentNode;
        }
        if (!rowStart) {
            std::cerr << "[ERROR]: Empty row." << std::endl;
            exit(1);
        }
        rowStart->left = prevNode;
        prevNode->right = rowStart;
    }
    currentNode = nullptr;
    ifs.close();

    /*Node* currentNode = nodeArray;
    while (!ifs.eof()) {
        std::string rowInput;
        std::getline(ifs, rowInput);
        if(rowInput.empty()){
            break;
        }
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
            std::cerr << "[ERROR]: Empty row." << std::endl;
            exit(1);
        }
        rowStart->left = currentNode - 1, (currentNode - 1)->right = rowStart;
    }*/


    Column* bestCol = nullptr;

    //Start of search(k) recursive procedure
    int level = 0;
    std::cout << "Starting RecursiveSearch.\n";
    RecursiveSearch(level, currentNode, bestCol);

    int smallestSetSize = RAND_MAX;
    int smallestSetIndex;
    /*for(int i = 0; i < rowSolutions.size(); ++i){
        for(int j = 0; j < rowSolutions[i].size(); ++j){
            if(rowSolutions[i].size() < smallestSetSize){
                smallestSetSize = rowSolutions[i].size();
                smallestSetIndex = i;
            }
        }
    }*/
    for(int i = 0; i < rowSoln.size(); ++i){
        for(int j = 0; j < rowSoln[i].size(); ++j){
            if(rowSoln[i].size() < smallestSetSize){
                smallestSetSize = rowSoln[i].size();
                smallestSetIndex = i;
            }
        }
    }

    std::cout << "Number of strips in smallest solution: " << smallestSetSize << std::endl;
    std::cout << "Smallest Solution:\n";
    for(const auto& subVec : rowSoln[smallestSetIndex]){
        for(const auto& v : subVec){
            std::cout << v->col->name << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    //std::cout << "All solutions:\n";
    /*for(const auto& solnVec : rowSolutions){
        for(const auto& subVec : solnVec){
            for(const auto& v : subVec){
                std::cout << v << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;*/

    if(solution == 0){
        std::cout << "No solutions found." << std::endl;
    }
    else{
        std::cout << solution << " solutions found." << std::endl;
        std::cout << "Highest level: " << highestLevel << std::endl;
    }


} //END INT MAIN


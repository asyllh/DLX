/*--------------/
ALH
constants.h
21/09/18
/--------------*/

#ifndef DLX_CONSTANTS_H
#define DLX_CONSTANTS_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

struct Timer {
    std::chrono::high_resolution_clock::time_point startTime, endTime;
    std::chrono::duration<float> totalTime;

    Timer(){
        startTime = std::chrono::high_resolution_clock::now();
    }

    ~Timer(){
        endTime = std::chrono::high_resolution_clock::now();
        totalTime = endTime - startTime;
        float totalTimems = totalTime.count() * 1000.0f;
        std::cout << "\nCPU Time: " << totalTimems << "ms (" << totalTime.count() << "s)" << std::endl;
    }
};

struct Column; //declare Column struct so that it can be used in the Node struct below

struct Node {
    Node* left; // Predecessor in row (node before this current node in row)
    Node* right; // Successor in row (node after this current node in row)
    Node* up; // Predecessor in column (node above this current node in column)
    Node* down; // Successor in column (node below this current node in column)
    Column* col; // The column containing this current node

    Node()
        : left(nullptr), right(nullptr), up(nullptr), down(nullptr), col(nullptr)
    {

    }
};

struct Column {
    int len; // The number of non-header items currently in this column's list
    std::string name;
    Node head; // The list header
    Column* prev; // The column before this current column
    Column* next; // The column after this current column

    Column()
        : len(0), name(""), head(), prev(nullptr), next(nullptr)
    {

    }
};

extern int solution;
extern int highestLevel;
extern Column* colRoot;
extern std::vector<Node*> choice;
extern std::vector<std::vector<std::vector<std::string> > > rowSolutions;
extern std::vector<std::vector<std::vector<Node*> > > rowSoln;

void ProgramInfo();


#endif

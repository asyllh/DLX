/*--------------/
ALH
constants.h
21/09/18
/--------------*/

#ifndef DLX_CONSTANTS_H
#define DLX_CONSTANTS_H

#include <iostream>
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
};

struct Column {
    int len; // The number of non-header items currently in this column's list
    char name[8]; // Name of column, used for printing
    Node head; // The list header
    Column* prev; // The column before this current column
    Column* next; // The column after this current column
};

extern const int maxLevel;
extern const int maxDegree;
extern const int maxCols;
extern const int maxNodes;
extern const int bufSize;


extern char buf[];
extern Node nodeArray[]; //place for nodes
extern Node* choice[]; //the row and column chosen on each level
extern Column colArray[]; //place for column records
extern Column root;


#endif

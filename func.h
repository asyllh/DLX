/*--------------/
ALH
func.h
07/09/18
/--------------*/

#ifndef DLX_FUNC_H
#define DLX_FUNC_H

/*//struct Column; //forward declare Column struct so that it can be used in Node struct below

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

void printRow(Node* p);

void printState(int lev);

// Cover column, block row, leaves all columns except column that is being covered, so a node is never removed from a list twice.
void cover(Column* c);

// Uncovering a column, done in exact reverse order of covering.
void uncover(Column* c);*/

#endif

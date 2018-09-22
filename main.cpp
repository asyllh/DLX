/*--------------/
ALH
C++ program implementing DLX
main.cpp
01/09/18
/--------------*/
/** TO DO **/
/* DONE: Make all global functions local, and add necessary arguments to functions
 * DONE: Make header file
 * DONE: Check if all macros are needed
 * nullptr
 * DONE: Remove need for labels
 * DONE: Change code so that it doesn't include 'primary' and other columns, all columns should be needed
   This involves the '|' symbol that would appear in an input file, change the code so that it doesn't
   separate primary and non primary columns
 * Use std::string instead of char pointers
 * Change method of reading input file, use ifstream/getline etc
 * Look out for need for null character '\0', might be needed
 * Check which variables are actually needed
 * DONE: Remove verbose
 * Program arguments
 * Make sure when running this main.cpp file that it is not still linked to the main.c file or dlx.cpp
 * Use try...catch and error functions to end program with message
 * How is the solution being stored? Just want to store which subset of rows contain exactly one 1 in every column
 */


#include <iostream>
#include <stdio.h> // printf, fprintf, sscanf, fgets, NULL, stdin, stderr <cstdio>
#include <stdlib.h> // exit, NULL <cstdlib>
#include <cctype> // isspace() function, <ctype.h>
#include <string>
#include <cstring> // strcmp, strlen <string.h>
#include "func.h"
using namespace std;

int main (int argc, char** argv) {

    char* p = nullptr;
    char* q = nullptr;
    Node* currentNode = nullptr;
    Column* bestCol = nullptr; // column chosen for branching
    Column* currentCol = nullptr;

    Timer timer;

    //region Inputting File
    //Reading columns from file
    currentCol = colArray + 1;
    std::cout << "Enter Column names:" << std::endl;
    fgets(buf, BUF_SIZE, stdin);
    if (buf[strlen(buf) - 1] != '\n') {
        std::cout << "Input line too long" << std::endl;
        exit(1);
    }
    for (p = buf; *p; ++p) { //no need for primary.
        while (isspace(*p)) { // isspace() is a function to check if the passed character is whitespace
            ++p;
        }
        if (!*p) {
            break;
        }
        for (q = p + 1; !isspace(*q); ++q);
        if (q > p + 7) {
            std::cout << "Column name too long." << std::endl;
            exit(1);
        }
        if (currentCol >= &colArray[MAX_COLS]) {
            std::cout << "Too many columns." << std::endl;
            exit(1);
        }
        for (q = currentCol->name; !isspace(*p); ++q, ++p) {
            *q = *p;
        }
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        currentCol->prev = currentCol - 1;
        (currentCol - 1)->next = currentCol;
        ++currentCol;
    }
    (currentCol - 1)->next = &colArray[0]; //&root
    colArray[0].prev = currentCol - 1; //root.prev


    // Reading rows from file
    currentNode = nodeArray;
    std::cout << "Enter Row: ";
    int x = 0;
    while (x < 6) {
        fgets(buf, BUF_SIZE, stdin);
        Node* rowStart = nullptr;
        Column* ccol = nullptr;


        if (buf[strlen(buf) - 1] != '\n') {
            std::cout << "Input line too long." << std::endl;
            exit(1);
        }
        for (p = buf; *p; ++p) {
            while (isspace(*p)) {
                ++p;
            }
            if (!*p) {
                break;
            }
            for (q = p + 1; !isspace(*q); ++q);
            if (q > p + 7) {
                std::cout << "Column name too long." << std::endl;
                exit(1);
            }
            for (q = currentCol->name; !isspace(*p); ++q, ++p) {
                *q = *p;
            }
            *q = '\0'; //End of string, null character
            for (ccol = colArray; strcmp(ccol->name, currentCol->name); ++ccol); // while ccolname and currentColname are DIFFERENT ++ccol
            // i.e. while strcmp( , ) == 1, which only happens when the names are different
            if (ccol == currentCol) {
                std::cout << "Unknown column name." << std::endl;
                exit(1);
            }
            if (currentNode == &nodeArray[MAX_NODES]) {
                std::cout << "Too many nodes." << std::endl;
                exit(1);
            }
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
        ++x;
    }
    //endregion

    p = nullptr;
    q = nullptr;
    currentNode = nullptr;
    bestCol = nullptr;
    currentCol = nullptr;

    //Start of search(k) recursive procedure
    int level = 0;
    recursiveSearch(level, currentNode, bestCol);

    for(int i = 0; i < 5; ++i){
        printRow(choice[i]);
    }





} //END INT MAIN


















    /*forward: ; // Set bestCol to the best column for branching:
        minLen = MAX_NODES;
        for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
            if(currentCol->len < minLen){
                bestCol = currentCol, minLen = currentCol->len;
            }
        }
        if(level > maxl){
            if(level >= MAX_LEVEL){
                cout << "Too many levels." << endl;
                exit(1);
            }
            maxl = level;
        }
        if(minLen > maxb){
            if(minLen >= maxDegree){
                cout << "Too many branches." << endl;
                exit(1);
            }
            maxb = minLen;
        }
        ++profile[level][minLen];
        cover(bestCol);
        currentNode = choice[level] = bestCol->head.down; //currentNode is first Node in bestCol column

    advance: ;
        if(currentNode == &(bestCol->head)){ //If all nodes in bestCol column have been assessed
            goto backup;
        }
        for(rowNode = currentNode->right; rowNode != currentNode; rowNode = rowNode->right){
            cover(rowNode->col);
        }
        if(root.next == &root){ //If root column is the only column left (all other columns have been covered)
            ++count;
            goto recover;
        }
        ++level;
        goto forward;

    backup: ;
        uncover(bestCol);
        if(level == 0){
            goto done;
        }
        --level;
        currentNode = choice[level];
        bestCol = currentNode->col;

    // Uncover all other columns of currentNode in the correct LIFO order
    recover:
        for(rowNode = currentNode->left; rowNode != currentNode; rowNode = rowNode->left){
            uncover(rowNode->col);
        }
        currentNode = choice[level] = currentNode->down;
        goto advance;

    done: ;
        cout << "Final column lengths:" << endl;
        for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
            cout << currentCol->name << "(" << currentCol->len << ")" << endl;
        }
        cout << endl;*/

    /*double tot, subtot;
    tot = 1; // The root node does not show up in the profile
    for(level = 1; level <= maxl + 1; ++level){
        subtot = 0;
        for(k = 0; k <= maxb; ++k){
            cout << profile[level][k];
            subtot += profile[level][k];
        }
        //cout << subtot << " nodes, " << updateProfile[level-1] << endl;
        tot += subtot;
    }
    cout << "Total " << tot << "nodes." << endl;*/


/* Backtracking through all solutions:
 * Strategy for generating all exact covers will be to repeatedly always choose
   the column that appears to be hardest to cover from all columns that still
   need to be covered.
 * This is the column with the shortest list. (i.e. smallest len)
 * All possibilites are then explored via depth-first search.

 * Depth-first search means last-in-first out maintenance of data structures.
 * There is no need for auxiliary tables to undelete elements from lists
   when backing up.
 * The nodes removed from doubly linked lists remember their former neighbours,
   because there is no garbage collection.

 * The basic operation is 'covering a column'.
 * This means removing it from the list of columns needing to be covered,
   and 'blocking' its rows, which is done by removing nodes from other
   lists whenever they belong to a row of a node in this column's list.
 */
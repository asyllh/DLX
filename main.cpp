/*--------------/
ALH
C++ program implementing DLX
main.cpp
01/09/18
/--------------*/
/** TO DO **/
/* Make all global functions local, and add necessary arguments to functions
 * Make header file
 * Check if all macros are needed
 * nullptr
 * Remove need for labels
 * Change code so that it doesn't include 'primary' and other columns, all columns should be needed
   This involves the '|' symbol that would appear in an input file, change the code so that it doesn't
   separate primary and non primary columns
 * Use std::string instead of char pointers
 * Change method of reading input file, use ifstream/getline etc
 * Look out for need for null character '\0', might be needed
 * Check which variables are actually needed
 * Remove verbose
 * Program arguments
 * Make sure when running this main.cpp file that it is not still linked to the main.c file or dlx.cpp
 * Use try...catch and error functions to end program with message
 * How is the solution being stored? Just want to store which subset of rows contain exactly one 1 in every column
 */

//#define maxLevel 150 // at most this many rows in a solution
//#define maxDegree 10000 // at most this many branches per search tree node
//#define maxCols 10000 // at most this many columns
//#define maxNodes 1000000 // at most this many nonzero elements in the matrix
//#define root colArray[0] // first column struct, head of list of columns, has no 'name', gateway to the unsettled columns
//#define bufSize 8 * maxCols + 3 // upper bound on the input line length

#include <iostream>
#include <stdio.h> // printf, fprintf, sscanf, fgets, NULL, stdin, stderr <cstdio>
#include <stdlib.h> // exit, NULL <cstdlib>
#include <cctype> // isspace() function, <ctype.h>
#include <cstring> // strcmp, strlen <string.h>
#include <chrono> // For Timer struct
//#include "constants.h"
#include "func.h"
using namespace std;



int main (int argc, char** argv) {

    char* p = nullptr;
    char* q = nullptr;
    int level;
    //int j, k, x;
    //int minLen;
    //Node* rowNode; // traverses a row
    Node* currentNode;
    Column* bestCol; // column chosen for branching
    Column* currentCol;

    Timer timer;

    //region Inputting File
    //Reading columns from file
    currentCol = colArray + 1;
    fgets(buf, bufSize, stdin);
    if (buf[strlen(buf) - 1] != '\n') {
        cout << "Input line too long" << endl;
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
            cout << "Column name too long." << endl;
            exit(1);
        }
        if (currentCol >= &colArray[maxCols]) {
            cout << "Too many columns." << endl;
            exit(1);
        }
        for (q = currentCol->name; !isspace(*p); ++q, ++p) {
            *q = *p;
        }
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        currentCol->prev = currentCol - 1, (currentCol - 1)->next = currentCol;
        ++currentCol;
    }
    (currentCol - 1)->next = &root, root.prev = currentCol - 1;


    // Reading rows from file
    currentNode = nodeArray;
    while (fgets(buf, bufSize, stdin)) {
        Node* rowStart = nullptr;
        Column* ccol = nullptr;

        if (buf[strlen(buf) - 1] != '\n') {
            cout << "Input line too long." << endl;
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
                cout << "Column name too long." << endl;
                exit(1);
            }
            for (q = currentCol->name; !isspace(*p); ++q, ++p) {
                *q = *p;
            }
            *q = '\0'; //End of string, null character
            for (ccol = colArray; strcmp(ccol->name, currentCol->name); ++ccol); // while ccolname and currentColname are DIFFERENT ++ccol
            // i.e. while strcmp( , ) == 1, which only happens when the names are different
            if (ccol == currentCol) {
                cout << "Unknown column name." << endl;
                exit(1);
            }
            if (currentNode == &nodeArray[maxNodes]) {
                cout << "Too many nodes." << endl;
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
            cout << "Empty row." << endl;
            exit(1);
        }
        rowStart->left = currentNode - 1, (currentNode - 1)->right = rowStart;
    }
    //endregion

    //Start of search(k) recursive procedure
    level = 0;
    //Function: recursiveSearch(level) here
    recursiveSearch(level, currentNode, bestCol);





} //END INT MAIN


















    /*forward: ; // Set bestCol to the best column for branching:
        minLen = maxNodes;
        for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
            if(currentCol->len < minLen){
                bestCol = currentCol, minLen = currentCol->len;
            }
        }
        if(level > maxl){
            if(level >= maxLevel){
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
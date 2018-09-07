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

#define maxLevel 150 // at most this many rows in a solution
#define maxDegree 10000 // at most this many branches per search tree node
#define maxCols 10000 // at most this many columns
#define maxNodes 1000000 // at most this many nonzero elements in the matrix
#define root colArray[0] // first column struct, head of list of columns, has no 'name', gateway to the unsettled columns
#define bufSize 8 * maxCols + 3 // upper bound on the input line length

#include <iostream>
#include <stdio.h> // printf, fprintf, sscanf, fgets, NULL, stdin, stderr <cstdio>
#include <stdlib.h> // exit, NULL <cstdlib>
#include <cctype> // isspace() function, <ctype.h>
#include <cstring> // strcmp, strlen <string.h>
#include <chrono> // For Timer struct
using namespace std;

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
        cout << "\nCPU Time: " << totalTimems << "ms (" << totalTime.count() << "s)" << endl;
    }
};

struct Column; //forward declare Column struct so that it can be used in Node struct below

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


// Global Variables:
/** MAKE THESE LOCAL, CHANGE FUNCTION ARGUMENTS **/
char buf[bufSize];
int level; // number of choices in current partial solution
int updates; // number of times we deleted a list element
int verbose; // >0 to show solutions, >1 to show partial ones too
int count = 0; // number of solutions found so far
int maxb = 0; // maximum branching factor actually needed
int maxl = 0; // maximum level actually reached
int profile[maxLevel][maxDegree]; // tree nodes of given level and degree
int updateProfile[maxLevel]; // updates at a given level
Node nodeArray[maxNodes]; //place for nodes
Node* choice[maxLevel]; //the row and column chosen on each level
Column colArray[maxCols + 2]; //place for column records



/* A row is identified not by name, but by the names of the columns it contains.
 * printRow outputs a row, given a pointer to any of its columns.
 * It also outputs the position of the row in its column.
 */
void printRow(Node *p){

    int k;
    Node* q = p;

    do {
        cout << q->col->name;
        q = q->right;
    } while (q != p);

    for(q = p->col->head.down, k=1; q != p; ++k){
        if(q == &(p->col->head)){
            cout << endl;
            return; //row not in its column! //return what??
        }
        else {
            q = q->down;
        }
    }
    cout << " (" << k << " of " << p->col->len << ")" << endl;
}

void printState(int lev){
    int l;
    for(l = 0; l <= lev; ++l){
        printRow(choice[l]);
    }
}

// Cover column, block row, leaves all columns except column that is being covered, so a node is never removed from a list twice.
void cover(Column *c){

    int k = 1; // updates
    Node* nn;
    Node* rr;
    Node* uu;
    Node* dd;
    Column* l;
    Column* r;

    l = c->prev;
    r = c->next;
    l->next = r;
    r->prev = l;

    for(rr = c->head.down; rr != &(c->head); rr = rr->down){
        for(nn = rr->right; nn != rr; nn = nn->right){
            uu = nn->up;
            dd = nn->down;
            uu->down = dd;
            dd->up = uu;
            ++k;
            --nn->col->len;
        }
    }
    updates += k;
    updateProfile[level] += k;
}

// Uncovering a column, done in exact reverse order of covering.
void uncover(Column *c){

    Node* nn;
    Node* rr;
    Node* uu;
    Node* dd;
    Column* l;
    Column* r;

    for(rr = c->head.up; rr!= &(c->head); rr = rr->up){
        for(nn = rr->left; nn != rr; nn = nn->left){
            uu = nn->up;
            dd = nn->down;
            uu->down = dd->up = nn;
            ++nn->col->len;
        }
    }
    l = c->prev;
    r = c->next;
    l->next = r->prev = c;
}


int main (int argc, char** argv){

    char* p;
    char* q;
    int j, k, x;
    int minLen;
    int primary;
    Node* pp; // traverses a row
    Node* currentNode;
    Column* currentCol;
    Column* bestCol; // column chosen for branching

    Timer timer;

    //Reading columns from file
    currentCol = colArray + 1;
    fgets(buf, bufSize, stdin);
    if(buf[strlen(buf)-1] != '\n'){
        cout << "Input line too long" << endl;
        exit(1);
    }
    for(p = buf; *p; ++p){ //no need for primary.
        while(isspace(*p)){ // isspace() is a function to check if the passed character is whitespace
            ++p;
        }
        if(!*p){
            break;
        }
        for(q = p+1; !isspace(*q); ++q);
        if(q > p+7){
            cout << "Column name too long." << endl;
            exit(1);
        }
        if(currentCol >= &colArray[maxCols]){
            cout << "Too many columns." << endl;
            exit(1);
        }
        for(q = currentCol->name; !isspace(*p); ++q, ++p){
            *q = *p;
        }
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        currentCol->prev = currentCol-1, (currentCol-1)->next = currentCol;
        ++currentCol;
    }
    (currentCol-1)->next = &root, root.prev = currentCol - 1;


    // Reading rows from file
    currentNode = nodeArray;
    while(fgets(buf, bufSize, stdin)){
        Node* rowStart = nullptr;
        Column* ccol;

        if(buf[strlen(buf)-1] != '\n'){
            cout << "Input line too long." << endl;
            exit(1);
        }
        for(p = buf; *p; ++p){
            while(isspace(*p)){
                ++p;
            }
            if(!*p){
                break;
            }
            for(q = p+1; !isspace(*q); ++q);
            if(q > p+7){
                cout << "Column name too long." << endl;
                exit(1);
            }
            for(q = currentCol->name; !isspace(*p); ++q, ++p){
                *q = *p;
            }
            *q = '\0'; //End of string, null character
            for(ccol = colArray; strcmp(ccol->name, currentCol->name); ++ccol); // while ccolname and currentColname are DIFFERENT ++ccol
            // i.e. while strcmp( , ) == 1, which only happens when the names are different
            if(ccol == currentCol){
                cout << "Unknown column name." << endl;
                exit(1);
            }
            if(currentNode == &nodeArray[maxNodes]){
                cout << "Too many nodes." << endl;
                exit(1);
            }
            if(!rowStart){
                rowStart = currentNode;
            }
            else{
                currentNode->left = currentNode -1, (currentNode -1)->right = currentNode;
            }
            currentNode->col = ccol;
            currentNode->up = ccol->head.up, ccol->head.up->down = currentNode;
            ccol->head.up = currentNode, currentNode->down = &ccol->head;
            ++ccol->len;
            ++currentNode;
        }
        if(!rowStart){
            cout << "Empty row." << endl;
            exit(1);
        }
        rowStart->left = currentNode - 1, (currentNode - 1)->right = rowStart;
    }


    //Function: search(level)
    level = 0;
    /** LINE 2: Choose a column object 'c' **/
    forward: ; // Set bestCol to the best column for branching:
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
        /** LINE 3: Cover column 'c' **/
        cover(bestCol);
        /** LINE 4: Setting r <- D[c] ready for the main for loop **/
        currentNode = choice[level] = bestCol->head.down; //currentNode is first Node in bestCol column

    advance: ;
        /** end main for loop when r = c, i.e. when currentNode is in bestCol **/
        if(currentNode == &(bestCol->head)){ //If all nodes in bestCol column have been assessed
            goto backup;
        }
        /** LINE 6: For each j <- R[r], R[R[r]], while j != r **/
        for(pp = currentNode->right; pp != currentNode; pp = pp->right){
            /** LINE 7: Cover column 'j' **/
            cover(pp->col);
        }
        /** LINE 1: If R[h] = h, print the current solution and return **/
        if(root.next == &root){ //If root column is the only column left (all other columns have been covered)
            ++count;
            goto recover;
        }
        /** LINE 8: search(k+1) **/
        ++level;
        goto forward;

    backup: ;
        /** LINE 12: Uncover column 'c' and return **/
        uncover(bestCol);
        /** END OF ALGORITHM: If top branch reached, no more columns to search, end **/
        if(level == 0){
            goto done;
        }
        --level;
        /** LINE 9: r<- O_k and c <- C[r] **/
        currentNode = choice[level];
        bestCol = currentNode->col;

    // Uncover all other columns of currentNode in the correct LIFO order
    recover:
        /** LINE 10: For each j <- L[r], L[L[r]], while j != r **/
        for(pp = currentNode->left; pp != currentNode; pp = pp->left){
            /** LINE 11: Uncover column 'j' **/
            uncover(pp->col);
        }
        /** LINE 4: Set r <- D[D[c]] (next node down in column) and go back into main for loop **/
        currentNode = choice[level] = currentNode->down;
        goto advance;

    done:
        if(verbose > 3){ // Print column lengths to make sure everything has been restored
            cout << "Final column lengths:" << endl;
            for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
                cout << currentCol->name << "(" << currentCol->len << ")" << endl;
            }
            cout << endl;
        }
    // End search(level)


    // Output at end of algorithm
    cout << "Altogether " << count << " solutions, after " << updates << " updates." << endl;
    double tot, subtot;
    tot = 1; // The root node does not show up in the profile
    for(level = 1; level <= maxl + 1; ++level){
        subtot = 0;
        for(k = 0; k <= maxb; ++k){
            cout << profile[level][k];
            subtot += profile[level][k];
        }
        cout << subtot << " nodes, " << updateProfile[level-1] << endl;
        tot += subtot;
    }
    cout << "Total " << tot << "nodes." << endl;

} //END INT MAIN



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
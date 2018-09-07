/*--------------/
ALH
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
 * Use std::string?
 * Change method of reading input file, use ifstream/getline etc
 * Look out for need for null character '\0', might be needed
 * Check which variables are actually needed
 * Remove verbose
 * Program arguments
 * Make sure when running this main.cpp file that it is not still linked to the main.c file or dlx.cpp
 * Use try...catch and error functions to end program with message
 */

/* Generalized exact cover.
 * Donald Knuth
 * This program implements algorithm X from Dancing Links paper.
 * Given a 0-1 matrix, the problem is to find all subsets of its rows whose
   sum is AT MOST 1 in all columns and EXACTLY 1 in all "primary" columns.

 * Matrix is specific in the standard input file as follows:
    * Each column has a symbolic name, between 1 and 7 characters long
    * The first line of input contains the names of all primary columns,
      followed by '|', followed by the names of all thee other columns.
    * If all columns are primary, the '|' may be omitted.
    * The remaining lines represent the rows, by listing the columns
      where 1 appears.
 * The program prints the number of solutions and the total number of link
   updates.
 * It also prints every 'n'th solution, if the integer command line
   argument 'n' is given.
 * A second command line argument causes the full search tree to be printed
 * A third command line argument makes the output even more verbose.
 */

#define maxLevel 150 // at most this many rows in a solution
#define maxDegree 10000 // at most this many branches per search tree node
#define maxCols 10000 // at most this many columns
#define maxNodes 1000000 // at most this many nonzero elements in the matrix

/* One 'column' struct is called the 'root'.
 * It serves as the head of the list of columns that need to be covered,
   and is identifiable by the fact that its 'name' is empty.
 */
#define root colArray[0] // gateway to the unsettled columns
#define bufSize 8 * maxCols + 3 // upper bound on the input line length


#include <iostream>
#include <stdio.h> //printf, fprintf, sscanf, fgets, NULL, stdin, stderr <cstdio>
#include <stdlib.h> //exit, NULL <cstdlib>
#include <cctype> //isspace() function, <ctype.h>
#include <cstring> //strcmp, strlen <string.h>

using namespace std;


/* Data structures:
 * Each column of the input matrix is represented by a 'column' struct.
 * Each row of the input matris is represented as a linked list of 'node' structs.
 * There is one node for each non-zero entry in the matrix.
 * The nodes are linked circularly within each row, in both directions.
 * The nodes are also linked circularly within each column.
 * The column lists each include a header node, but the rows lists do not.
 * Column header nodes are part of a 'column' struct, which contains further
   information about the column.
 */

/* Each node contains five fields.
   * Four are the pointers of doubly linked lists
   * The fifth points to the column containing the node
 */

/* Each 'column' struct contains five fields.
   * The 'head' is a node that stands at the head of its list of nodes.
   * The 'len' tells the length of that list of nodes, not counting the header.
   * The 'name' is a user specified identifier.
   * 'next' and 'prev' point to adjacent columns, when this column is part
     of a doubly linked list.
 * As backtracking proceeds, nodes will be deleted from column lists when
   their row has been blocked by other rows in the partial solution.
 * When backtracking is complete, the data structures will be restored to
   their original state.
 */

//Each Node is linked to a Node on its left, a Node to its right, a Node above, and a Node below.
//Each Node is also part of a Column.

struct Column; //forward declare Column struct so that it can be used in Node struct below

struct Node {
    Node* left; // Predecessor in row (node before this current node in row)
    Node* right; // Successor in row (node after this current node in row)
    Node* up; // Predecessor in column (node above this current node in column)
    Node* down; // Successor in column (node below this current node in column)
    Column* col; // The column containing this current node
};

//Each Column has a length, a name, a header which is a Node.
//Each Column also has a Nolumn to its left (prev) and a Column to its right (next)
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
int spacing = 1; // if |verbose|, we output solutions when |count%spacing==0|
int profile[maxLevel][maxDegree]; // tree nodes of given level and degree
int updateProfile[maxLevel]; // updates at a given level
Node nodeArray[maxNodes]; //place for nodes
Node* choice[maxLevel]; //the row and column chosen on each level
Column colArray[maxCols + 2]; //place for column records


// Subroutines:
/* Output a row to screen:
 * A row is identified not by name, but by the names of the columns it contains.
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

/* Covering a column:
 * When a row is blocked, it leaves all lists except the list of the column
   that is being covered.
 * Therefore, a node is never removed from a list twice.
 */

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


/* Uncovering a column:
 * Uncovering a column is done in precisely the reverse order.
 * The pointers thereby execute a 'dance' which returns them
   to their former state.
 */
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

    verbose = argc - 1;
    if (verbose){
        sscanf(argv[1], "%d", &spacing); //
    }

    // Inputting the matrix - read the column names:
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



    // Inputting the matrix - read the rows:
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

    level = 0;
    forward: ; // Set bestCol to the best column for branching:
        minLen = maxNodes;
        if(verbose > 2){
            cout << "Level: " << level;
        }
        for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
            /*if(verbose > 2){
                cout << " " << currentCol->name << "(" << currentCol->len << ")";
            }*/
            if(currentCol->len < minLen){
                bestCol = currentCol, minLen = currentCol->len;
            }
        }
        if(verbose){
            if(level > maxl){
                if(level >= maxLevel) {
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
            if(verbose > 2){
                cout << "Branching on " << bestCol->name << "(" << minLen << ")" << endl;
            }
        }
        cover(bestCol);
        currentNode = choice[level] = bestCol->head.down;

    advance: ;
        if(currentNode == &(bestCol->head)){
            goto backup;
        }
        if(verbose > 1){
            cout << "Level " << level << ": ";
            printRow(currentNode);
        }

        // cover all other columns of currentNode
        for(pp = currentNode->right; pp != currentNode; pp = pp->right){
            cover(pp->col);
        }

        if(root.next == &root){ //Record solution and goto recover
            ++count;
            if(verbose){
                ++profile[level+1][0];
                if(count % spacing == 0){
                    cout << count << endl;
                    for(k = 0; k <= level; ++k){
                        printRow(choice[k]);
                    }
                }
            }
            goto recover;
        }
        ++level;
        goto forward;

    backup:
        uncover(bestCol);
        if(level == 0){
            goto done;
        }
        --level;
        currentNode = choice[level];
        bestCol = currentNode->col;

    // uncover all other columns of currentNode
    /* We included left links, thereby making the rows doubly linked,
       so that columns would be uncovered in the correct LIFO order
       in this part of the program.
     * The 'uncover' routine itself could have done its job with
       right links only.
     */

    recover:
        for(pp = currentNode->left; pp != currentNode; pp = pp->left){
            uncover(pp->col);
        }
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
    // End backtracking

    cout << "Altogether " << count << " solutions, after " << updates << " updates." << endl;
    if(verbose){ // Print a profile of the search tree
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
    }
} //END INT MAIN
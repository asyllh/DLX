/*Generalized exact cover.
This program implements the algorithm discussed in my paper about ``dancing
 links.'' I hacked it together from the {\mc XCOVER} program that I wrote in
1994; I apologize for not having time to apply spit and polish.

Given a matrix whose elements are 0 or 1, the problem is to
find all subsets of its rows whose sum is at most~1 in all columns and
{\it exactly\/}~1 in all ``primary'' columns. The matrix is specified
in the standard input file as follows: Each column has a symbolic name,
from one to seven characters long. The first line of input contains
the names of all primary columns, followed by `\.{\char"7C}', followed by
the names of all other columns.
(If all columns are primary, the~`\.{\char"7C}' may be omitted.)
The remaining lines represent the rows, by listing the columns where 1 appears.

The program prints the number of solutions and the total number of link
updates. It also prints every $n$th solution, if the integer command
line argument $n$ is given. A second command-line argument causes the
full search tree to be printed, and a third argument makes the output
even more verbose.
*/

#define maxLevel 150 // at most this many rows in a solution
#define maxDegree 10000 // at most this many branches per search tree node
#define maxCols 10000 // at most this many columns
#define maxNodes 1000000 // at most this many nonzero elements in the matrix
#define verbose Verbose // kludge because of 64-bit madness in SGB library
#define root colArray[0] // gateway to the unsettled columns
#define bufSize 8 * maxCols + 3 // upper bound on the input line length
#define panic(m) { fprintf(stderr, "%s!\n%s", m, buf); exit(-1); }

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


//Type Definitions
typedef struct nodeStruct{
    struct nodeStruct *left, *right; //predecessor and successor in row
    struct nodeStruct *up, *down; //predecessor and successor in column
    struct colStruct *col; // the column containing this node
} node;

typedef struct colStruct{
    node head; //the list header
    int len; //the number of non-header items currently in this column's list
    char name[8]; //name of column, used for printing
    struct colStruct *prev, *next; //neighbours of this column
} column;



//Global Variables
int verbose; // $>0$ to show solutions, $>1$ to show partial ones too
long long count = 0; // number of solutions found so far
double updates; // number of times we deleted a list element
int spacing = 1; // if |verbose|, we output solutions when |count%spacing==0|
double profile[maxLevel][maxDegree]; // tree nodes of given level and degree
double updateProfile[maxLevel]; // updates at a given level
int maxb = 0; // maximum branching factor actually needed
int maxl = 0; // maximum level actually reached

column colArray[maxCols + 2]; //place for column records
node nodeArray[maxNodes]; //place for nodes
char buf[bufSize];

int level; // number of choices in current partial solution
node *choice[maxLevel]; //the row and column chosen on each level



// Subroutines
void printRow(p)
    node *p;
{
    register node *q = p;
    register int k;
    do {
        printf(" %s", q->col->name);
        q = q->right;
    } while (q != p);

    for(q = p->col->head.down, k=1; q != p; k++){
        if(q == &(p->col->head)){
            printf("\n");
            return; //row not in its column!
        }
        else {
            q = q->down;
        }
    }
    printf(" (%d of %d)\n", k, p->col->len);
}


void printState(int lev){
    register int l;
    for(l = 0; l <= lev; l++){
        printRow(choice[l]);
    }
}

void cover(c) // Covering column
    column *c;
{
    register column *l, *r;
    register node *rr, *nn, *uu, *dd;
    register k = l; // updates
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
            k++;
            nn->col->len--;
        }
    }
    updates += k;
    updateProfile[level] += k;
}

void uncover(c)
    column *c;
{
    register column *l, *r;
    register node *rr, *nn, *uu, *dd;
    for(rr = c->head.up; rr!= &(c->head); rr = rr->up){
        for(nn = rr->left; nn != rr; nn = nn->left){
            uu = nn->up;
            dd = nn->down;
            uu->down = dd->up = nn;
            nn->col->len++;
        }
    }
    l = c->prev;
    r = c->next;
    l->next = r->prev = c;
}



int main (argc, argv)
    int argc;
    char *argv[];
{
    //Local Variables
    register column *currentCol;
    register char *p, *q;
    register node *currentNode;
    register column *bestCol; // column chosen for branching
    register node *pp; // traverses a row
    register int minLen;
    register int j, k, x;
    int primary;

    verbose = argc - 1;
    if (verbose){
        sscanf(argv[1], "%d", &spacing);
    }

    currentCol = colArray + 1;
    fgets(buf, bufSize, stdin);
    if(buf[strlen(buf)-1] != '\n'){
        panic("Input line too long");
    }
    for(p = buf, primary = 1; *p; p++){
        while(isspace(*p)){ // isspace() is a function to check if the passed character is whitespace
            p++;
        }
        if(!*p){
            break;
        }
        if(*p == '|'){
            primary = 0;
            if(currentCol == colArray + 1){
                panic("No primary columns");
            }
            (currentCol - 1)->next = &root,root.prev = currentCol-1;
            continue;
        }
        for(q = p+1; !isspace(*q); q++);
        if(q > p+7){
            panic("Column name too long");
        }
        if(currentCol >= &colArray[maxCols]){
            panic("Too many columns");
        }
        for(q = currentCol->name; !isspace(*p); q++, p++){
            *q = *p;
        }
        currentCol->head.up = currentCol->head.down = &currentCol->head;
        currentCol->len = 0;
        if(primary){
            currentCol->prev = currentCol-1, (currentCol - 1)->next = currentCol;
        }
        else{
            currentCol->prev = currentCol->next = currentCol;
        }
        currentCol++;
    }
    if(primary){
        if(currentCol == colArray + 1){
            panic("No primary columns");
        }
        (currentCol-1)->next = &root, root.prev = currentCol - 1;
    }

    currentNode = nodeArray;
    while(fgets(buf, bufSize, stdin)){
        register column *ccol;
        register node *rowStart;
        if(buf[strlen(buf)-1] != '\n'){
            panic("Input line too long");
        }
        rowStart = NULL;
        for(p = buf; *p; p++){
            while(isspace(*p)){
                p++;
            }
            if(!*p){
                break;
            }
            for(q = p+1; !isspace(*q); q++);
            if(q > p+7){
                panic("Column name too long");
            }
            for(q = currentCol->name; !isspace(*p); q++, p++){
                *q = *p;
            }
            *q = '\0';
            for(ccol = colArray; strcmp(ccol->name, currentCol->name); ccol++);
            if(ccol == currentCol){
                panic("Unknown column name");
            }
            if(currentNode == &nodeArray[maxNodes]){
                panic("Too many nodes");
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
            ccol->len++;
            currentNode++;
        }
        if(!rowStart){
            panic("Empty row");
        }
        rowStart->left = currentNode -1, (currentNode -1)->right = rowStart;
    }

















} //END INT MAIN













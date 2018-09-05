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
#define verbose Verbose // kludge because of 64-bit madness in SGB library

/* One 'column' struct is called the 'root'.
 * It serves as the head of the list of columns that need to be covered,
   and is identifiable by the fact that its 'name' is empty.
 */
#define root colArray[0] // gateway to the unsettled columns


#define bufSize 8 * maxCols + 3 // upper bound on the input line length
#define panic(m) { fprintf(stderr, "%s!\n%s", m, buf); exit(-1); }

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


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

// Type Definitions:
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



// Global Variables:
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


// Subroutines:

/* Output a row to screen:
 * A row is identified not by name, but by the names of the columns it contains.
 * PrintRow outputs a row, given a pointer to any of its columns.
 * It also outputs the position of the row in its column.
 */

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


void PrintState(int lev){
    register int l;
    for(l = 0; l <= lev; l++){
        printRow(choice[l]);
    }
}

/* Covering a column:
 * When a row is blocked, it leaves all lists except the list of the column
   that is being covered.
 * Therefore, a node is never removed from a list twice.
 */
void cover(c) // Covering column
    column *c;
{
    register column *l, *r;
    register node *rr, *nn, *uu, *dd;
    register k = 1; // updates
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


/* Uncovering a column:
 * Uncovering a column is done in precisely the reverse order.
 * The pointers thereby execute a 'dance' which returns them
   to their former state.
 */
void uncover(column *c)
    //column *c;
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


// Main function:

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

    // Inputting the matrix - read the column names:
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

    // Inputting the matrix - read the rows:
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


    /* Backtracking through all solutions:
     * Strategy for generating all exact covers will be to repeatedly always choose
       the column that appears to be hardest to cover from all columns that still
       need to be covered.
     * This is the column with the shortest list.
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
            printf("Level %d", level);
        }
        for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
            if(verbose > 2){
                printf(" %s(%d)", currentCol->name, currentCol->len);
            }
            if(currentCol->len < minLen){
                bestCol = currentCol, minLen = currentCol->len;
            }
        }
        if(verbose){
            if(level > maxl){
                if(level >= maxLevel){
                    panic("Too many levels");
                    maxl = level;
                }
            }
            if(minLen > maxb){
                if(minLen >= maxDegree){
                    panic("Too many branches");
                    maxb = minLen;
                }
            }
            profile[level][minLen]++;
            if(verbose > 2){
                printf("branching on %s(%d)\n", bestCol->name, minLen);
            }
        }

        cover(bestCol);
        currentNode = choice[level] = bestCol->head.down;

    advance: ;
        if(currentNode == &(bestCol->head)){
            goto backup;
        }
        if(verbose > 1){
            printf("L%d:",level);
            printRow(currentNode);
        }
        // Cover all other columns of currentNode
        for(pp = currentNode->right; pp != currentNode; pp = pp->right){
            cover(pp->col);
        }

        if(root.next == &root){ //Record solution and goto recover
            count++;
            if(verbose){
                profile[level+1][0]++;
                if(count % spacing == 0){
                    printf("%lld:\n", count);
                    for(k = 0; k <= level; k++){
                        printRow(choice[k]);
                    }
                }
            }
            goto recover;
        }
        level++;
        goto forward;

    backup: uncover(bestCol);
        if(level == 0){
            goto done;
        }
        level--;
        currentNode = choice[level];
        bestCol = currentNode->col;

    recover: // Uncover all other columns of currentNode
        /* We included left links, thereby making the rows doubly linked,
           so that columns would be uncovered in the correct LIFO order
           in this part of the program.
         * The 'uncover' routine itself could have done its job with
           right links only.
         */
        for(pp = currentNode->left; pp != currentNode; pp = pp->left){
            uncover(pp->col);
        }


        currentNode = choice[level] = currentNode->down;
        goto advance;

    done:
        if(verbose > 3){ // Print column lengths to make sure everything has been restored
            printf("Final column lengths");
            for(currentCol = root.next; currentCol != &root; currentCol = currentCol->next){
                printf(" %s(%d)", currentCol->name, currentCol->len);
            }
            printf("\n");
        }
    // End backtracking

    printf("Altogether %lld solutions, after %.15g updates. \n", count, updates);
    if(verbose){ // Print a profile of the search tree
        double tot, subtot;
        tot = 1; // The root node does not show up in the profile
        for(level = 1; level <= maxl + 1; level++){
            subtot = 0;
            for(k = 0; k <= maxb; k++){
               printf(" %5.6g", profile[level][k]);
               subtot += profile[level][k];
            }
            printf(" %5.15g nodes, %.15g updates\n", subtot, updateProfile[level-1]);
            tot += subtot;
        }
        printf("Total %.15g nodes.\n", tot);
    }
    exit(0);
} //END INT MAIN
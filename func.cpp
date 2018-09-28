/*--------------/
ALH
func.cpp
07/09/18
/--------------*/
#include <iostream>
#include "func.h"

/* A row is identified not by name, but by the names of the columns it contains.
 * printRow outputs a row, given a pointer to any of its columns.
 * It also outputs the position of the row in its column.
 */
void PrintRow(Node* p){

    int i;
    Node* q = p;

    do {
        std::cout << q->col->name << " ";
        q = q->right;
    } while (q != p);


    for(q = p->col->head.down, i=1; q != p; ++i){
        if(q == &(p->col->head)){
            std::cout << std::endl;
            return; //row not in its column!
        }
        else {
            q = q->down;
        }
    }
    std::cout << std::endl;
    //std::cout << " (" << i << " of " << p->col->len << ")" << std::endl;
}

void StoreRowSolution(int level, Node* choice[]){

    int i;
    std::vector<std::vector<std::string> > soln;
    std::vector<std::string> temp;
    for(i = 0; i <= level; ++i){
        Node* p = choice[i];
        Node* q = p;
        do{
            temp.push_back(q->col->name);
            q = q->right;
        } while (q != p);
        soln.push_back(temp);
        temp.clear();
    }

    rowSolutions.push_back(soln);

}


// Cover column, block row, leaves all columns except column that is being covered, so a node is never removed from a list twice.
void Cover(Column* c){

    int i = 1; // updates
    Node* nn = nullptr;
    Node* rr = nullptr;
    Node* uu = nullptr;
    Node* dd = nullptr;
    Column* l = nullptr;
    Column* r = nullptr;

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
            ++i;
            --nn->col->len;
        }
    }
    //updates += k;
    // updateProfile[level] += k;
}

// Uncovering a column, done in exact reverse order of covering.
void Uncover(Column* c){

    Node* nn = nullptr;
    Node* rr = nullptr;
    Node* uu = nullptr;
    Node* dd = nullptr;
    Column* l = nullptr;
    Column* r = nullptr;

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

//Function to choose column object c = 'bestCol', should return pointer to bestCol only.
void SelectBestColumn(Column*& bestCol){

    int minLen = 100000;
    //Column root1 = colArray[0];
    for(Column* currCol = colArray[0].next; currCol != &colArray[0]; currCol = currCol->next){
        if(currCol->len < minLen){
            bestCol = currCol, minLen = currCol->len;
        }
    }

}

void RecursiveSearch(int& level, Node*& currentNode, Column*& bestCol){
    //static int i = 0;
    //std::cout << "In level: " << level << std::endl;
    //++i;

    /* Function: Choose column object 'bestCol'
     * &bestCol, Column* currentCol (internal only), int minLen (internal only), MAX_NODES, root. */
    SelectBestColumn(bestCol); // Returns bestCol pointer (line 2)

    Cover(bestCol); // Cover bestCol column (line 3)

    //Set r <- D[c] and O_k <- r, starting from first node below head node of column (line 4/5)
    currentNode = choice[level] = bestCol->head.down;

    // While r != c, continue going down column until head node is reached (line 4)
    while(currentNode != &(bestCol->head)){
        // For each j <- R[r] ... while j!=r, cover column j (line 6/7)
        for(Node* rowNode = currentNode->right; rowNode != currentNode; rowNode = rowNode->right){
            Cover(rowNode->col);
        }
        // Do search(k+1) if root is not the only column left
        if(colArray[0].next != &colArray[0]){
            ++level;
            if(level > highestLevel){
                highestLevel = level;
            }
            RecursiveSearch(level, currentNode, bestCol);
        }
        else if(colArray[0].next == &colArray[0]){
            ++solution;
            /*for(int i = 0; i <= level; ++i){
                PrintRow(choice[i]);
            }*/
            StoreRowSolution(level, choice);
        }
        // For each j <- L[r],... while j!=r, uncover column j (line 10/11)
        for(Node* rowNode = currentNode->left; rowNode != currentNode; rowNode = rowNode->left){
            Uncover(rowNode->col);
        }

        currentNode = choice[level] = currentNode->down; // Set currentNode to the next node down in the column
    }

    //Exit while loop when all nodes in column have been assessed, currentNode == head node of bestCol
    Uncover(bestCol);
    // Go up a level, exit this search function and go back into the previous search function
    if(level > 0){
        --level;
        currentNode = choice[level];
        bestCol = currentNode->col;
    }
    else{ // level == 0, top level reached, end function
        return;
    }
    // This code will end here if level == 0, or will go back into previous recursiveSearch function (approx line 214) if level > 0.

}

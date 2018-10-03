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

void StoreRowSolution(int level){ //no need to pass choice as it's global

    std::vector<std::vector<std::string> > soln;
    std::vector<std::string> temp;
    std::vector<Node*>::iterator it;

    for(it = choice.begin(); it != choice.begin() + (level+1); ++it){
        Node* first = *it;
        Node* n = first;
        do{
            //std::cout << n->col->name << " ";
            temp.push_back(n->col->name);
            n = n->right;
        } while (n != first);
        //std::cout << std::endl;
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

    int minLen = RAND_MAX;
    //Column root1 = colArray[0];
    //for(Column* currCol = colArray[0].next; currCol != &colArray[0]; currCol = currCol->next){
    for(Column* currCol = colRoot->next; currCol != colRoot; currCol = currCol->next){
        if(currCol->len < minLen){
            bestCol = currCol, minLen = currCol->len;
        }
    }

}

void RecursiveSearch(int& level, Node*& currentNode, Column*& bestCol){
    //std::cout << "In level: " << level << std::endl;

    /* Function: Choose column object 'bestCol'
     * &bestCol, Column* currentCol (internal only), int minLen (internal only), MAX_NODES, root. */
    SelectBestColumn(bestCol); // Returns bestCol pointer (line 2)

    Cover(bestCol); // Cover bestCol column (line 3)

    //Set r <- D[c] and O_k <- r, starting from first node below head node of column (line 4/5)
    //currentNode = choice[level] = bestCol->head.down;
    if(choice.size() > level){
        currentNode = choice[level] = bestCol->head.down;
    }
    else if(choice.size() <= level){
        choice.push_back(bestCol->head.down);
        currentNode = bestCol->head.down;
    }

    // While r != c, continue going down column until head node is reached (line 4)
    while(currentNode != &(bestCol->head)){
        // For each j <- R[r] ... while j!=r, cover column j (line 6/7)
        for(Node* rowNode = currentNode->right; rowNode != currentNode; rowNode = rowNode->right){
            Cover(rowNode->col);
        }
        // Do search(k+1) if root is not the only column left
        //if(colArray[0].next != &colArray[0]){
        if(colRoot->next != colRoot){
            ++level;
            if(level > highestLevel){
                highestLevel = level;
            }
            RecursiveSearch(level, currentNode, bestCol);
        }
        //else if(colArray[0].next == &colArray[0]){
        else if(colRoot->next == colRoot){
            ++solution;
            StoreRowSolution(level); //no need to pass choice as it's global
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

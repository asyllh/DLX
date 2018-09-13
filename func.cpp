/*--------------/
ALH
func.cpp
07/09/18
/--------------*/
#include "func.h"
#include <iostream>

/*void printRow(Node* p){

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
}*/

/*void printState(int lev){
    int l;
    for(l = 0; l <= lev; ++l){
        printRow(choice[l]);
    }
}*/

// Cover column, block row, leaves all columns except column that is being covered, so a node is never removed from a list twice.
/*void cover(Column* c){

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
    //updates += k;
    // updateProfile[level] += k;
}*/

// Uncovering a column, done in exact reverse order of covering.
/*
void uncover(Column* c){

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
}*/

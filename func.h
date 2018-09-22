/*--------------/
ALH
func.h
07/09/18
/--------------*/

#ifndef DLX_FUNC_H
#define DLX_FUNC_H

#include "constants.h"



void printRow(Node* p);

void cover(Column* c);

void uncover(Column* c);

void selectBestColumn(Column*& bestCol);

void recursiveSearch(int& level, Node*& currentNode, Column*& bestCol);

#endif

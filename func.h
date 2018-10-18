/*--------------/
ALH
func.h
07/09/18
/--------------*/

#ifndef DLX_FUNC_H
#define DLX_FUNC_H

#include "constants.h"
void Test(int level);

void StoreRowSolution(int level);

void Cover(Column* c);

void Uncover(Column* c);

void SelectBestColumn(Column*& bestCol);

void RecursiveSearch(int& level, Node*& currentNode, Column*& bestCol);

#endif

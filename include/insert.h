#ifndef INSERT_H
#define INSERT_H

#include "types.h"
#include "streedata.h"
#include "stree_aux.h"
#include "basedef.h"
#include "externs.h"

// Insert a leaf edge from the current base vertex
void insert(STree *stree);

// Insert inner veretx and split appropriate edges
void split_and_insert(STree *stree);

#endif

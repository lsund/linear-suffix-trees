#ifndef INSERT_H
#define INSERT_H

#include "types.h"
#include "streedata.h"
#include "stree_aux.h"
#include "basedef.h"
#include "externs.h"

// Insert a leaf edge from the current base vertex
void insert_leaf(STree *stree);

// Insert inner veretx and split appropriate edges
void insert_inner(STree *stree);

#endif

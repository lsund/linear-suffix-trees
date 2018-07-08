#ifndef INSERT_H
#define INSERT_H

#include "types.h"
#include "streedef.h"
#include "spaceman.h"
#include "config.h"
#include "text.h"
#include "stree.h"

// Insert a leaf edge from the current base vertex
void insert(STree *st);

// Insert inner veretx and split appropriate edges
void split_and_insert(STree *st);

#endif

#ifndef SKIP_COUNT_H
#define SKIP_COUNT_H

#include "streedata.h"
#include "stree_aux.h"
#include "basedef.h"
#include "externs.h"

// Find  the location of the current head by only looking at the first
// character of each edge.
void skip_count(STree *stree);

#endif

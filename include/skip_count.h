#ifndef SKIP_COUNT_H
#define SKIP_COUNT_H

#include "streedata.h"
#include "externs.h"

// Find  the location of the current head by only looking at the fst
// character of each edge.
void skip_count(STree *st);

#endif

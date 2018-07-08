#ifndef INSERT_H
#define INSERT_H

#include "types.h"
#include "streedef.h"
#include "spaceman.h"
#include "config.h"
#include "text.h"
#include "stree.h"

// Insert a leaf edge from the current base vertex. The label is marked with
// tail(i) and the new vertex gets `st->ls.nxt_ind` as leaf number.
void insert(STree *st);

// Insert a leaf edge from the current base, which is not a vertex. If the
// base is on the edge A ---> B, then this edge is split into A ---> X ---> B
// and a new leaf vertex is inserted under X. The labels are updated
// accordingly.
void split_and_insert(STree *st);

#endif

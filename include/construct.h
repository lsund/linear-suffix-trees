#ifndef CONSTRUCT_H
#define CONSTRUCT_H


#include "bitvector.h"
#include "spaceman.h"
#include "stree.h"
#include "basedef.h"
#include <stdbool.h>

void init(Suffixtree *stree, wchar_t *text, Uint textlen);

// Slow-scan
void scanprefix(Suffixtree *stree);

// skip-count
void rescan(Suffixtree *stree);

// Insert a large node
void completelarge(Suffixtree *stree);

// Insert a leaf edge from the current base vertex
void insertleaf(Suffixtree *stree);

// Insert inner veretx and split appropriate edges
void insertbranchnode(Suffixtree *stree);

// Construct the successor chain for the children of the root. This is done of
// the end of the algorithm in one sweep.
void linkrootchildren(Suffixtree *stree);

void freestree(Suffixtree *stree);

#endif

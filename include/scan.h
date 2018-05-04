#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "stree.h"
#include "basedef.h"
#include "util.h"
#include "location.h"
#include "stree_aux.h"

Wchar *scan(STree *stree, Loc *loc, Uint *start_vertex, Pattern patt);

void walk(STree *stree);

#define MATCHED(X, R, L)        (X) == (Uint) ((R) - (L) + 1)

#endif

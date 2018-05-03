#ifndef STREE_AUX_H
#define STREE_AUX_H

#include "streedata.h"
#include "bitvector.h"

#define SMALLINTS           3                  // # of integers for small node
#define LARGEINTS           5                  // # of integers for large node

#define DIVBYSMALLINTS(V)   ((V) / SMALLINTS)  // div by SMALLINTS
#define MULTBYSMALLINTS(V)  ((V) * SMALLINTS)  // multiply by SMALLINTS

#define SMALLBIT            FIRSTBIT       // mark small node
#define NILBIT              FIRSTBIT       // mark nil reference in brother
#define MAXINDEX            (NILBIT - 1)     // all except for first bit
#define MAXDISTANCE         MAXINDEX       // maximal distance value

#define ISLARGE(V)                (!((V) & SMALLBIT))

#define GETDEPTH(B)               (*((B)+2))
#define GETHEADPOS(B)             (*((B)+3))
#define GETDISTANCE(B)            (*((B)+2))
#define GETCHAINEND(C, B, D)      C = (B) + MULTBYSMALLINTS(D)

#define START_ALLOCSIZE         max(0.5 * MULTBYSMALLINTS(textlen + 1), 48);
#define EXTRA_ALLOCSIZE         max(0.05 * MULTBYSMALLINTS(textlen + 1), 48);

#define GETSUFFIXLINK(B)          getlargelinkconstruction(stree)

// Get info for branch vertex
Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

Uint get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance);

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep);

Uint get_depth_after_head(STree *stree, Uint *vertexp, Uint *distance, Uint **largep);

void follow_link(STree *stree);

#endif

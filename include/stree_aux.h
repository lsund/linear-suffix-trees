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

// Get info for branch vertex
Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

void get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance, Uint *head);

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep);


#endif

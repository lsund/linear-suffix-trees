#ifndef LOCATION_H
#define LOCATION_H

#include "streedata.h"
#include "types.h"

// A location is implemented by the type `Loc`
typedef struct
{
    String string;     // string represented by location
    Uint *prev;    // reference to previous node (which is branching)
    Wchar *first;            // first character
    Uint edgelen;           // length of edge
    Uint remain;            // number of remaining characters on edge
    Uint *next;   // reference to node the edge points to
} Loc;

// If a location is a node u, we set `remain` to 0, and store a reference to
// u in `next`. Moreover, we store a position where u starts and its length
// in `string`. If the location is of form (u, v, w, uvw), then the
// components of the location satisfies the following values:
//
// 1. `prev` is a reference to u
//
// 3. edgelen = |vw|
//
// 4. remain = |w|
//
// 5. next is a reference to uvw
//
// Since w is not empty, a location is a node location iff remain = 0.

///////////////////////////////////////////////////////////////////////////////
// Functions

void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc);

void make_loc(STree *stree, Uint leafnum, Uint plen, Loc *loc, Wchar *first);

void update_loc(Uint *next, Uint start, Uint plen, Wchar *first, Uint depth, Uint edgelen, Loc *loc);

#endif

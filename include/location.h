#ifndef LOCATION_H
#define LOCATION_H

#include <stdbool.h>

#include "streedef.h"
#include "types.h"


// A struct representing a string, with its start and length
typedef struct {
  Uint start, length;
} String;

// A location is implemented by the type `Loc`
typedef struct
{
    String string;           // string represented by location
    Uint *prev;              // reference to previous node (which is branching)
    Wchar *fst;            // fst character
    Uint edgelen;            // length of edge
    Uint remain;             // number of remaining characters on edge
    Uint *nxt;              // reference to node the edge points to
    bool leafedge;           // Is the location on a leafedge
} Loc;

// If a location is a node u, we set `remain` to 0, and store a reference to
// u in `nxt`. Moreover, we store a position where u starts and its length
// in `string`. If the location is of form (u, v, w, uvw), then the
// components of the location satisfies the following values:
//
// 1. `prev` is a reference to u
//
// 3. edgelen = |vw|
//
// 4. remain = |w|
//
// 5. nxt is a reference to uvw
//
// Since w is not empty, a location is a node location iff remain = 0.

///////////////////////////////////////////////////////////////////////////////
// Functions

void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc);

void make_loc(STree *st, Uint leafnum, Uint plen, Loc *loc, Wchar *fst);

void update_loc(Uint *nxt, Uint start, Uint plen, Wchar *fst, Uint depth, Uint edgelen, Loc *loc);

#endif

#ifndef LOCATION_H
#define LOCATION_H

#include <stdbool.h>

#include "streedef.h"
#include "types.h"
#include "text.h"


// A struct representing a string, with its start and length
typedef struct {
  Uint start, length;
} String;

// A Loc is a location in the suffix tree.
//
// * If the location is a node u, then
//
// rem : 0
// nxt : u
// s   : the string for where u starts
// d   : the depth of this string
//
// * If the location is an edge, then v is the string before the location and w
// the string after the location on that string. The parent is represented by u
// and thus the child by uvw.
//
// edgelen  : |vw|
// prev     : u
// rem      : |w|
// nxt      : uvw
//
// Since w is not empty is a location a vertex iff remain = 0;
typedef struct
{
    Uint s;             // string represented by location
    Uint d;             // The depth of the location, ie. the length of the string.
    Uint *prev;         // reference to previous node (which is branching)
    Wchar *fst;         // fst character
    Uint edgelen;       // length of edge
    Uint rem;           // number of remaining characters on edge
    VertexP nxt;        // reference to node the edge points to
    bool isleaf;        // Is the location on a leafedge
} Loc;

///////////////////////////////////////////////////////////////////////////////
// Functions

void init_location(Uint *v, Uint hd, Uint depth, Loc *loc);

void make_loc(STree *st, Uint s, Uint plen, Loc *loc, Wchar *fst);

void update_loc(
        VertexP nxt,
        Uint s,
        Uint plen,
        Wchar *fst,
        Uint d,
        Uint edgelen,
        Loc *loc
    );

#endif

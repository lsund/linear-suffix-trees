#ifndef STREEDEF_H
#define STREEDEF_H

#include "types.h"

///////////////////////////////////////////////////////////////////////////////
// Types

typedef Uint Vertex;

typedef Uint *VertexP;

typedef struct pattern {
    Wchar *start;
    Wchar *end;
} Pattern;

typedef struct table {
    Uint *fst;
    Uint *nxt;
    Uint nxt_ind;
    Uint size;
    Uint *alloc;
} Table;


typedef struct chain {
    VertexP fst;
    Uint size;
} Chain;


typedef struct label {
    Wchar *start;
    Wchar *end;
} Label;

// The edge that contains the end of head
//
// The right component of the head location uv.
// head.label.start refers to the fst character of v
// headend to the last character. In case, v = empty
// headend = null
typedef struct head {
    VertexP vertex;  // the vertex u
    Label label;
    Uint depth;
} Head;


// The edge which is to be splitted
typedef struct split {
    // refers to the vertex where the split edge ends
    Vertex child;
    // refers to the branching node to the left of the inner node to be
    // inserted
    Vertex left;
} SplitEdge;


typedef struct stree {

    Table is;           // Inner vertices
    Table ls;           // Leaf vertices
    VertexP rs;         // references to successors of root

    Uint c_depth;       // Current depth of the newly inserted inner vertex

    SplitEdge split;    // A reference to the child and left sibling of the
                        // current edge split by the algorithm

    Chain chain;        // Address of the vertex current chains starts at,
                        // and the total size of the chain

    Head hd;          // Representing the edge containing the head location.

    Wchar *tail;        // Points to the tail

} STree;

#endif

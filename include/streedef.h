#ifndef STREEDEF_H
#define STREEDEF_H

#include "types.h"

// The root is at the first address in the inner table
#define ROOT                    (st->is.fst)
// The root children, unique with respect to a character
#define ROOT_CHILD(C)           (st->rs[(Uint) (C)])

// To retrieve an index for an inner vertex, subtract the root
#define REF_TO_INDEX(P)         ((Uint) ((P) - st->is.fst))
// To retrieve an index for a leaf, subtract the first leaf
#define LEAFREF_TO_INDEX(P)     ((Uint) ((P) - st->ls.fst))

// To get the corresponding inner vertex reference, add its index to the first
// reference.
#define VERTEX_TO_REF(V)        st->is.fst + VERTEX_TO_INDEX((V)) // address
// To get the corresponding leaf reference, add its index to the first
// reference.
#define VERTEX_TO_LEAFREF(V)    st->ls.fst + VERTEX_TO_INDEX((V))

typedef struct pattern {
    Wchar *start;
    Wchar *end;
} Pattern;

typedef struct table {
    VertexP fst;
    VertexP nxt;
    Uint nxt_ind;
    Uint size;
    VertexP alloc;
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
    VertexP v;  // the vertex u
    Label l;
    Uint d;
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

    Wchar *tl;        // Points to the tail

} STree;

#endif

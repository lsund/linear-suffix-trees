#ifndef STREEDEF_H
#define STREEDEF_H

#include "types.h"
#include "text.h"

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

// A Pattern is a search pattern. A pattern has a start and an end.
typedef struct pattern {
    Wchar *start;
    Wchar *end;
} Pattern;

// A Table represents an array for storing vertices.
typedef struct table {
    // The address of the first occupied vertex.
    VertexP fst;
    // The address of the next, unoccupied vertex.
    VertexP nxt;
    // The next unoccupied index.
    Uint nxt_ind;
    // The number of vertices stored.
    Uint size;
    // The next unoccupied address, such that a large vertex can be stored.
    VertexP alloc;
} Table;

// A HeadEdge represents The edge that contains the head location. In case this
// location is on the last character of the edge then the end of the label is
// NULL.
typedef struct head {
    // The parent vertex of the edge
    VertexP v;
    // The characters from the head location to the end of the edge.
    Label l;
    // The depth of the head location
    Uint d;
} HeadEdge;

// A SplitEdge represents the edge that is to be splitted after finding the new
// head location.
typedef struct split {
    // refers to the vertex where the split edge ends
    Vertex child;
    // refers to the branching node to the left of the inner node to be
    // inserted.
    Vertex left;
} SplitEdge;

// A Chain is a sequence of small vertices terminating in a large vertex. The
// structure stores a reference to the address of the first vertex in the chain
// as well as the number of vertices in the chain.
typedef struct chain {
    VertexP fst;
    Uint size;
} Chain;


// A STree represents a suffix tree.
typedef struct stree {

    // The table of inner vertices.
    Table is;
    // The table of leaf vertices.
    Table ls;

    // The list of root children.
    VertexP rs;

    // The current depth of the most newly inserted inner vertex.
    Uint c_depth;

    // The current chain of small vertices.
    Chain chain;

    // The edge to be split after finding a new head location.
    SplitEdge split;

    // The edge containing the current head in the tree
    HeadEdge hd;

    // Points to the tail
    Wchar *tl;

} STree;

#endif

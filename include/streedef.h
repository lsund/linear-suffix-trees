#ifndef STREEDEF_H
#define STREEDEF_H

#include "types.h"
#include "arraydef.h"

///////////////////////////////////////////////////////////////////////////////
// Types


typedef Uint Vertex;

typedef Uint *VertexP;

typedef struct table {
    Uint *fst;
    Uint *nxt;
    Uint nxt_ind;
    Uint size;
} Table;


typedef struct text {
    Wchar *content;                // points to the input string
    Wchar *sentinel;               // points to the position of the \(\$\)-symbol
    Uint len;                      // the length of the input string
    Uint alphasize;                // the number of different characters in t
} Text;


typedef struct chain {
    VertexP fst;
    Uint size;
} Chain;


typedef struct label {
    Wchar *start;
    Wchar *end;
} Label;

typedef struct head {
    Label label;
    Uint depth;
    VertexP vertex;  // the vertex u
} Head;

// The edge that contains the end of head
//
// The right component of the head location uv.
// head.label.start refers to the fst character of v
// headend to the last character. In case, v = empty
// headend = null


// The edge which is to be splitted
typedef struct split {
    // refers to the vertex where the split edge ends
    Vertex child;
    // refers to the branching node to the left of the inner node to be
    // inserted
    Vertex left;
} SplitEdge;


typedef struct suffixtree {

    Uint *rootchildren;         // references to successors of root
    Table is;
    Table ls;

    Uint current_branchdepth;          // depth of the new branch node

    SplitEdge split;

    Chain chain;                // address of the node current chains starts at

    Head head;
    Wchar *tail;            // points to the tail

    // refers to the last address, such that at
    // least LARGE_VERTEXSIZE integers are available. So a large node can be
    // stored in the available amount of space.
    Uint *allocated;

} STree;

#endif

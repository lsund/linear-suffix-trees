#ifndef STREEDATA_H
#define STREEDATA_H

#include "types.h"
#include "arraydef.h"

///////////////////////////////////////////////////////////////////////////////
// Types

typedef struct pattern {
    Wchar *start;
    Wchar *end;
} Pattern;


// A suffix tree is implemented by the type SuffixTree. Most of the feilds are
// only used during construction.
//
// The important are the following:

typedef struct table {

    Uint *first;
    Uint *next_free;
    Uint next_free_num;
    Uint size;

} Table;


typedef struct text {
    Wchar *content;               // points to the input string
    Wchar *sentinel;           // points to the position of the \(\$\)-symbol
    Uint len;               // the length of the input string
} Text;


typedef struct suffixtree {

    Uint textlen;               // the length of the input string
    Wchar *text;               // points to the input string
    Wchar *sentinel;           // points to the position of the \(\$\)-symbol


    Table inner_vertices;
    Table leaf_vertices;

    Uint *rootchildren;         // references to successors of root

    Uint headnodedepth;         // the depth of the headnode
    Uint insertnode;            // the node the split edge leads to
    Uint insertprev;            // the edge preceeding the split edge
    Uint smallnotcompleted;     // the number of small nodes in the current chain

    Uint onsuccpath;            // refers to node on success path of headnode
    Uint currentdepth;          // depth of the new branch node
    Uint branchnodeoffset;      // number of leafs in tree
    Uint alphasize;             // the number of different characters in t
    Uint maxbranchdepth;        // maximal depth of branching node
    Uint largenode;             // number of large nodes
    Uint smallnode;             // number of small nodes
    Uint *setlink;              // address of a nil-reference
    Uint *chainstart;           // address of the node current chains starts at

    Uint *headnode;             // left component of head location

    Uint *alloc_leftbound;    // refers to the last address, such that at
    // least LARGEINTS integers are
    // available. So a large node can be stored in
    // the available amount of space.
    Uint *nonmaximal;           // bit table: if node with headposition \(i\) is
    // not maximal, then \(nonmaximal[i]\) is set.
    Uint *leafcounts;           // holds counts of the number of leafs in subtree
    // indexed by headposition
    Bool setatnewleaf;          // nil-reference is stored in new leaf
    Wchar *headstart;          // these references represent the right component
    Wchar *headend;            // of the head location \((\overline{u},v)\).
    // \emph{headstart} refers to the first character
    // of \(v\), and \emph{headend} to the last
    // character. In case, \(v=\varepsilon\),
    // \(\emph{headend}=\emph{NULL}\).
    Wchar *tailptr;            // points to the tail

} STree;

#endif

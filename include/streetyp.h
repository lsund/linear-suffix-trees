/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef STREETYP_H
#define STREETYP_H

#include "types.h"
#include "arraydef.h"

// A reference consists of an `address` pointing to a leaf or a branching node.
// The boolean toleaf is true iff `address` points to a leaf.
typedef struct
{
  Bool toleaf;
  Uint *address;
} Reference;

// Reference to a branching node
typedef Uint * Bref;

// Reference to a leaf
typedef Uint * Lref;

// For each branching node we store six values
typedef struct
{
  Uint headposition;        // the head position of the branching node
  Uint depth;               // the depth of the branching node
  Bref suffixlink;          // the suffix link is always to a branching node
  Reference firstchild;     // the reference to the first child
  Reference branchbrother;  // the reference to the right brother;
                            // if this doesn't exist then it's \texttt{NULL}
} Branchinfo;


// A suffix tree is implemented by the type SuffixTree. Most of the feilds are
// only used during construction.
//
// The important are the following:
//
// wchar_t *text;     // points to the input string
// Uint textlen;     // the length of the input string
// Uint *branchtab;  // stores the infos for the branching nodes
// Uint *leaftab;    // stores the brother-references of the leaves
typedef struct suffixtree
{
  Uint textlen;               // the length of the input string
  Uint *leaftab;              // stores the brother-references of the leafs
  Uint *branchtab;            // table TBranch
  Uint *rootchildren;         // references to successors of root

  wchar_t *text;               // points to the input string
  wchar_t *sentinel;           // points to the position of the \(\$\)-symbol

  Uint nextfreeleafnum;       // the number of the next leaf
  Uint headnodedepth;         // the depth of the headnode
  Uint insertnode;            // the node the split edge leads to
  Uint insertprev;            // the edge preceeding the split edge
  Uint smallnotcompleted;     // the number of small nodes in the current chain
  Uint nextfreebranchnum;     // the number of the next free branch node
  Uint onsuccpath;            // refers to node on success path of headnode
  Uint currentdepth;          // depth of the new branch node
  Uint branchnodeoffset;      // number of leafs in tree
  Uint alphasize;             // the number of different characters in t
  Uint maxbranchdepth;        // maximal depth of branching node
  Uint largenode;             // number of large nodes
  Uint smallnode;             // number of small nodes
  Uint *setlink;              // address of a nil-reference
  Uint *nextfreeleafptr;      // points to next free entry in leaftab
  Uint *chainstart;           // address of the node, current chains starts at
  Uint *nextfreebranch;       // reference to next free base addr. in branchtab
  Uint *headnode;             // left component of head location
  Uint currentbranchtabsize;  // current number of cells in branchtab
  Uint *firstnotallocated;    // refers to the last address, such that at
                              // least \texttt{LARGEINTS} integers are
                              // available. So a large node can be stored in
                              // the available amount of space.
  Uint *nonmaximal;           // bit table: if node with headposition \(i\) is
                              // not maximal, then \(nonmaximal[i]\) is set.
  Uint *leafcounts;           // holds counts of the number of leafs in subtree
                              // indexed by headposition
  Bool setatnewleaf;          // nil-reference is stored in new leaf
  wchar_t *headstart;          // these references represent the right component
  wchar_t *headend;            // of the head location \((\overline{u},v)\).
                              // \emph{headstart} refers to the first character
                              // of \(v\), and \emph{headend} to the last
                              // character. In case, \(v=\varepsilon\),
                              // \(\emph{headend}=\emph{NULL}\).
  wchar_t *tailptr;            // points to the tail

#if (wchar_tBYTES == 2) || (wchar_tBYTES == 4)
  Sint lastcharindex;
#endif

} Suffixtree;

DECLAREARRAYSTRUCT(Bref);

// A location is implemented by the type `Location`
typedef struct
{
  String locstring;     // string represented by location
  Bref previousnode;    // reference to previous node (which is branching)
  wchar_t *firstptr;     // pointer to first character of edge label
  Uint edgelen;         // length of edge
  Uint remain;          // number of remaining characters on edge
  Reference nextnode;   // reference to node the edge points to
} Location;

// If a location is a node u, we set `remain` to 0, and store a reference to
// u in `nextnode`. Moreover, we store a position where u starts and its length
// in `locstring`. If the location is of form (u, v, w, uvw), then the
// components of the location satisfies the following values:
//
// 1. `previousnode` is a reference to u
//
// 2. firstptr points to the first symbol of the edge label vw
//
// 3. edgelen = |vw|
//
// 4. remain = |w|
//
// 5. nextnode is a reference to uvw
//
// Since w is not empty, a location is a node location iff remain = 0.

// A SimpleLoc is a subset of a Location
typedef struct {
  Uint remain;
  Uint textpos;  // these last two items are redundant and can be computed
  Reference nextnode;
} Simpleloc;     // \Typedef{Simpleloc}

DECLAREARRAYSTRUCT(Simpleloc);

// A path in the suffix tree is stored with the `Pathinfo` struct
typedef struct
{
  Uint depth, headposition;
  Bref ref;
} Pathinfo;

DECLAREARRAYSTRUCT(Pathinfo);

typedef struct
{
  Bool secondtime;
  ArrayBref stack;
} DFSstate;

#endif

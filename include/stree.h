/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */


#ifndef STREE_H
#define STREE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>
#include <limits.h>
#include <stdbool.h>

#include "basedef.h"
#include "spaceman.h"
#include "clock.h"
#include "externs.h"
#include "types.h"
#include "arraydef.h"
#include "bitvector.h"

///////////////////////////////////////////////////////////////////////////////
// Types

// Reference to leaf node
typedef Uint * Lref;

// For each branching node we store six values
typedef struct
{
    Uint headposition;        // the head position of the branching node
    Uint depth;               // the depth of the branching node
    Uint *suffixlink;          // the suffix link is always to a branching node
    Uint *firstchild;     // the reference to the first child
    Uint * branchbrother;  // the reference to the right brother;
    // if this doesn't exist then it's \texttt{NULL}
} Branchinfo;


// A suffix tree is implemented by the type SuffixTree. Most of the feilds are
// only used during construction.
//
// The important are the following:
//
// Wchar *text;     // points to the input string
// Uint textlen;     // the length of the input string
// Uint *inner_vertices;  // stores the infos for the branching nodes
// Uint *leaftab;    // stores the brother-references of the leaves

typedef struct table {

    Uint *first;
    Uint *next_free;
    Uint next_free_num;
    Uint size;

} Table;


typedef struct suffixtree
{

    Table inner_vertices;
    Table leaf_vertices;

    Uint textlen;               // the length of the input string

    Uint *rootchildren;         // references to successors of root

    Wchar *text;               // points to the input string
    Wchar *sentinel;           // points to the position of the \(\$\)-symbol

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
    // least \texttt{LARGEINTS} integers are
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

#if (WcharBYTES == 2) || (WcharBYTES == 4)
    Sint lastcharindex;
#endif

} STree;

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

// A path in the suffix tree is stored with the `Pathinfo` struct
typedef struct
{
    Uint depth, headposition;
    Uint *ref;
} Pathinfo;

DECLAREARRAYSTRUCT(Pathinfo);

typedef struct
{
    Bool secondtime;
    ArrayUint stack;
} DFSstate;

///////////////////////////////////////////////////////////////////////////////
// Functions

Sint construct(STree *stree,Wchar *text,Uint textlen);

Sint constructprogressstree(STree *stree,Wchar *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info);

void setdepthheadposition(STree *stree,Uint depth, Uint headposition);

void setsuffixlink(STree *stree,Uint slink);

Uint getlargelinkconstruction(STree *stree);

void init(STree *stree, Wchar *text, Uint textlen);

// Slow-scan
void scanprefix(STree *stree);

// skip-count
void rescan(STree *stree);

// Insert a large node
void completelarge(STree *stree);

// Insert a leaf edge from the current base vertex
void insertleaf(STree *stree);

// Insert inner veretx and split appropriate edges
void insertbranchnode(STree *stree);

// Construct the successor chain for the children of the root. This is done of
// the end of the algorithm in one sweep.
void linkrootchildren(STree *stree);

void freestree(STree *stree);

///////////////////////////////////////////////////////////////////////////////
// Macros

#define SMALLINTS           3                  // # of integers for small node
#define LARGEINTS           5                  // # of integers for large node
#define MULTBYSMALLINTS(V)  ((V) * SMALLINTS)  // multiply by SMALLINTS
#define DIVBYSMALLINTS(V)   ((V) / SMALLINTS)  // div by SMALLINTS

#define SMALLBIT            FIRSTBIT       // mark small node
#define NILBIT              FIRSTBIT       // mark nil reference in brother
#define MAXINDEX            (NILBIT - 1)     // all except for first bit
#define MAXDISTANCE         MAXINDEX       // maximal distance value

/*
   We use the least significant bit to discriminate references to leafs
   and branching nodes. Since base addresses are even, the unset least
   significant bit of a reference identifies a base address. For a leaf
   reference we shift the leaf number one to the right and
   set the least significant bit.
   */

#define IS_LEAF(V)                 ((V) & LEAFBIT)
#define ISLARGE(V)                (!((V) & SMALLBIT))
#define MAKELEAF(V)               ((V) | LEAFBIT)
#define MAKELARGE(V)              (V)
#define MAKELARGELEAF(V)          MAKELEAF(V)

#define LEAF_INDEX(V)           ((V) & ~(LEAFBIT | SMALLBIT))
#define GETBRANCHINDEX(V)         LEAF_INDEX(V)

#define IS_NOTHING(P)                 ((P) & NILBIT)
#define UNDEFREFERENCE            (~((Uint) 0))
#define MAXTEXTLEN                ((MAXINDEX/((LARGEINTS+SMALLINTS)/2)) - 3)

#define GETCHILD(B)               ((*(B)) & MAXINDEX)
#define GETBROTHER(B)             (*((B)+1))
#define GETDISTANCE(B)            (*((B)+2))
#define GETDEPTH(B)               (*((B)+2))
#define GETHEADPOS(B)             (*((B)+3))
#define GETSUFFIXLINK(B)          getlargelinkconstruction(stree)
#define SETCHILD(B,VAL)           SETVAL(B,((*(B)) & SMALLBIT) | (VAL))
#define SETBROTHER(B,VAL)         SETVAL(B+1,VAL)

#define SETDISTANCE(B,VAL)        SETVAL(B+2,VAL);\
    SETVAL(B,(*(B)) | SMALLBIT)
#define SETDEPTHHEADPOS(DP,HP)    SETVAL(stree->inner_vertices.next_free+2,DP);\
    SETVAL(stree->inner_vertices.next_free+3,HP)

#define SETNEWCHILD(B,VAL)        SETVAL(B,VAL)
#define SETNEWCHILDBROTHER(CH,BR) SETVAL(stree->inner_vertices.next_free,CH);\
    SETVAL(stree->inner_vertices.next_free+1,BR)

#define SETSUFFIXLINK(SL)         SETVAL(stree->inner_vertices.next_free+4,SL)

#define LEAFBROTHERVAL(V)         (V)
#define SETLEAFBROTHER(B,VAL)     *(B) = (VAL)

#define GETCHAINEND(C, B, D)      C = (B) + MULTBYSMALLINTS(D)
#define MAKEBRANCHADDR(V)         (V)
#define SETBRANCHNODEOFFSET       /* nothing */

#define ROOT(ST)            ((ST)->inner_vertices.first)

#define IS_ROOT(ST, V)          ((ST)->inner_vertices.first == V)

#define IS_UNDEF(V)          ((V) == UNDEFREFERENCE)

// Is the location the root?
#define ROOTLOCATION(LOC)\
    (((LOC)->string.length == 0) ? True : False)

// Index of a branch and leaf relative to the first address
#define INDEX_INNER(ST,A)      ((Uint) ((A) - ROOT(ST)))
#define LEAFADDR2NUM(ST,A)    ((Uint) ((A) - (ST)->leaf_vertices.first))

// For each branching node we store five integers. These can be accessed by
// some or-combination.
#define ACCESSDEPTH          UintConst(1)
#define ACCESSHEADPOS        (UintConst(1) << 1)
#define ACCESSSUFFIXLINK     (UintConst(1) << 2)
#define ACCESSFIRSTCHILD     (UintConst(1) << 3)
#define ACCESSBRANCHBROTHER  (UintConst(1) << 4)

#define SETVAL(E,VAL) *(E) = VAL

// Retrieves the depth and headposition of the branching vertex PT.
#define GETBOTH(DP,HP,PT) \
    if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
{\
    distance = 1 + \
    DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (PT)));\
    DP = stree->currentdepth + distance;\
    HP = stree->leaf_vertices.next_free_num - distance;\
} else\
{\
    if(ISLARGE(*(PT)))\
    {\
        DP = GETDEPTH(PT);\
        HP = GETHEADPOS(PT);\
    } else\
    {\
        distance = GETDISTANCE(PT);\
        GETCHAINEND(largep,PT,distance);\
        DP = GETDEPTH(largep) + distance;\
        HP = GETHEADPOS(largep) - distance;\
    }\
}

#define GETONLYHEADPOS(HP,PT) \
    if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
{\
    distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (PT)));\
    HP = stree->leaf_vertices.next_free_num - distance;\
} else\
{\
    if(ISLARGE(*(PT)))\
    {\
        HP = GETHEADPOS(PT);\
    } else\
    {\
        distance = GETDISTANCE(PT);\
        GETCHAINEND(largep,PT,distance);\
        HP = GETHEADPOS(largep) - distance;\
    }\
}

#define GETONLYDEPTH(DP,PT) \
    if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
{\
    distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (PT)));\
    DP = stree->currentdepth  + distance;\
} else\
{\
    if(ISLARGE(*(PT)))\
    {\
        DP = GETDEPTH(PT);\
    } else\
    {\
        distance = GETDISTANCE(PT);\
        GETCHAINEND(largep,PT,distance);\
        DP = GETDEPTH(largep) + distance;\
    }\
}

#define GETDEPTHAFTERHEADPOS(DP,PT) \
    if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
{\
    DP = stree->currentdepth + distance;\
} else\
{\
    if(ISLARGE(*(PT)))\
    {\
        DP = GETDEPTH(PT);\
    } else\
    {\
        DP = GETDEPTH(largep) + distance;\
    }\
}

#define FOLLOWSUFFIXLINK\
    if(ISLARGE(*(stree->headnode)))\
{\
    stree->headnode = stree->inner_vertices.first + GETSUFFIXLINK(stree->headnode);\
} else\
{\
    stree->headnode += SMALLINTS;\
}\
stree->headnodedepth--

// Set the address for a nil-reference. In the case the reference is a new
// leaf, this is marked
#define RECALLNEWLEAFADDRESS(A)   stree->setlink = A;\
                                                   stree->setatnewleaf = True
#define RECALLLEAFADDRESS(A)      stree->setlink = A;\
                                                   stree->setatnewleaf = False
#define RECALLBRANCHADDRESS(A)    stree->setlink = (A) + 1;\
                                                   stree->setatnewleaf = False

#define SETNILBIT                 *(stree->setlink) = NILBIT

#define SETMAXBRANCHDEPTH(D)      if((D) > stree->maxbranchdepth)\
{\
    stree->maxbranchdepth = D;\
}

#define LEADLEVEL 2

#define SHOWINDEX(NODE) /* Nothing */

#define CHECKADDR(ST,A) /* Nothing */

#endif

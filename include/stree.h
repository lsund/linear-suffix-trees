/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
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

  Uint textlen;               // the length of the input string
  Uint *leaftab;              // stores the brother-references of the leafs

  /* Uint *inner_vertices;            // table for inner vertices */

  Uint *rootchildren;         // references to successors of root

  wchar_t *text;               // points to the input string
  wchar_t *sentinel;           // points to the position of the \(\$\)-symbol

  Uint nextfreeleafnum;       // the number of the next leaf
  Uint headnodedepth;         // the depth of the headnode
  Uint insertnode;            // the node the split edge leads to
  Uint insertprev;            // the edge preceeding the split edge
  Uint smallnotcompleted;     // the number of small nodes in the current chain

  /* Uint next_free_inner_num;     // the number of the next free branch node */

  Uint onsuccpath;            // refers to node on success path of headnode
  Uint currentdepth;          // depth of the new branch node
  Uint branchnodeoffset;      // number of leafs in tree
  Uint alphasize;             // the number of different characters in t
  Uint maxbranchdepth;        // maximal depth of branching node
  Uint largenode;             // number of large nodes
  Uint smallnode;             // number of small nodes
  Uint *setlink;              // address of a nil-reference
  Uint *nextfreeleafptr;      // points to next free entry in leaftab
  Uint *chainstart;           // address of the node current chains starts at

  /* Uint *next_free_inner;      // reference to next free base addr. in inner_vertices */

  Uint *headnode;             // left component of head location

  /* Uint inner_vertices_size;  // current number of cells in inner_vertices */

  Uint *alloc_leftbound;    // refers to the last address, such that at
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

///////////////////////////////////////////////////////////////////////////////
// Functions

Sint constructstree(Suffixtree *stree,wchar_t *text,Uint textlen);

Sint constructprogressstree(Suffixtree *stree,wchar_t *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info);

void setdepthheadposition(Suffixtree *stree,Uint depth, Uint headposition);

void setsuffixlink(Suffixtree *stree,Uint slink);

Uint getlargelinkconstruction(Suffixtree *stree);

void init(Suffixtree *stree, wchar_t *text, Uint textlen);

// Slow-scan
void scanprefix(Suffixtree *stree);

// skip-count
void rescan(Suffixtree *stree);

// Insert a large node
void completelarge(Suffixtree *stree);

// Insert a leaf edge from the current base vertex
void insertleaf(Suffixtree *stree);

// Insert inner veretx and split appropriate edges
void insertbranchnode(Suffixtree *stree);

// Construct the successor chain for the children of the root. This is done of
// the end of the algorithm in one sweep.
void linkrootchildren(Suffixtree *stree);

void freestree(Suffixtree *stree);

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

#define ISLEAF(V)                 ((V) & LEAFBIT)
#define ISLARGE(V)                (!((V) & SMALLBIT))
#define MAKELEAF(V)               ((V) | LEAFBIT)
#define MAKELARGE(V)              (V)
#define MAKELARGELEAF(V)          MAKELEAF(V)

#define GETLEAFINDEX(V)           ((V) & ~(LEAFBIT | SMALLBIT))
#define GETBRANCHINDEX(V)         GETLEAFINDEX(V)

#define NILPTR(P)                 ((P) & NILBIT)
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

// Is the location the root?
#define ROOTLOCATION(LOC)\
        (((LOC)->locstring.length == 0) ? True : False)

// Index of a branch and leaf relative to the first address
#define INDEX_INNER(ST,A)      ((Uint) ((A) - ROOT(ST)))
#define LEAFADDR2NUM(ST,A)    ((Uint) ((A) - (ST)->leaftab))

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
          HP = stree->nextfreeleafnum - distance;\
        } else\
        {\
          if(ISLARGE(*(PT)))\
          {\
            DP = GETDEPTH(PT);\
            HP = GETHEADPOS(PT);\
          } else\
          {\
            distance = GETDISTANCE(PT);\
            GETCHAINEND(largeptr,PT,distance);\
            DP = GETDEPTH(largeptr) + distance;\
            HP = GETHEADPOS(largeptr) - distance;\
          }\
        }

#define GETONLYHEADPOS(HP,PT) \
        if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
        {\
          distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (PT)));\
          HP = stree->nextfreeleafnum - distance;\
        } else\
        {\
          if(ISLARGE(*(PT)))\
          {\
            HP = GETHEADPOS(PT);\
          } else\
          {\
            distance = GETDISTANCE(PT);\
            GETCHAINEND(largeptr,PT,distance);\
            HP = GETHEADPOS(largeptr) - distance;\
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
            GETCHAINEND(largeptr,PT,distance);\
            DP = GETDEPTH(largeptr) + distance;\
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
            DP = GETDEPTH(largeptr) + distance;\
          }\
        }

#define GETHEADPOSAFTERDEPTH(HP,PT) \
        if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
        {\
          HP = stree->nextfreeleafnum - distance;\
        } else\
        {\
          if(ISLARGE(*(PT)))\
          {\
            HP = GETHEADPOS(PT);\
          } else\
          {\
            HP = GETHEADPOS(largeptr) - distance;\
          }\
        }

#define NEXTNODE(PT)\
        if(ISLARGE(*(PT)))\
        {\
          PT += LARGEINTS;\
        } else\
        {\
          PT += SMALLINTS;\
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

#define RECALLSUCC(S)             /* Nothing */

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

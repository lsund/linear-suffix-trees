/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

//}

//\FILEINFO{streehuge.h}{Stefan Kurtz}{March 2003}

//\Ignore{

#ifndef STREEHUGE_H
#define STREEHUGE_H

#include "types.h"
#include "debug.h"
#include "streetyp.h"
#include "stree.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_ALLOC           5000000

#define SMALLINTS           3                  // # of integers for small node
#define LARGEINTS           5                  // # of integers for large node
#define MULTBYSMALLINTS(V)  ((V) * SMALLINTS)  // multiply by SMALLINTS
#define DIVBYSMALLINTS(V)   ((V) / SMALLINTS)  // div by SMALLINTS

#define LEAFBIT             SECONDBIT      // mark leaf address
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
#define SETDEPTHHEADPOS(DP,HP)    SETVAL(stree->nextfreebranch+2,DP);\
                                  SETVAL(stree->nextfreebranch+3,HP)

#define SETNEWCHILD(B,VAL)        SETVAL(B,VAL)
#define SETNEWCHILDBROTHER(CH,BR) SETVAL(stree->nextfreebranch,CH);\
                                  SETVAL(stree->nextfreebranch+1,BR)

#define SETSUFFIXLINK(SL)         SETVAL(stree->nextfreebranch+4,SL)

#define LEAFBROTHERVAL(V)         (V)
#define SETLEAFBROTHER(B,VAL)     *(B) = (VAL)

#define GETCHAINEND(C,B,D)        C = (B) + MULTBYSMALLINTS(D)
#define MAKEBRANCHADDR(V)         (V)
#define SETBRANCHNODEOFFSET       /* nothing */

#ifdef DEBUG
#define CHILDREFERSTOLEAF(B)   ISLEAF(*(B))
#endif

void setdepthheadposition(Suffixtree *stree,Uint depth, Uint headposition);

void setsuffixlink(Suffixtree *stree,Uint slink);

Uint getlargelinkconstruction(Suffixtree *stree);

#endif

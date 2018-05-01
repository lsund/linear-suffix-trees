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

#include "construct.h"
#include "access.h"
#include "debug.h"
#include "spaceman.h"
#include "clock.h"
#include "externs.h"

Sint constructstree(Suffixtree *stree,SYMBOL *text,Uint textlen);

Sint constructprogressstree(Suffixtree *stree,SYMBOL *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info);

void setdepthheadposition(Suffixtree *stree,Uint depth, Uint headposition);

void setsuffixlink(Suffixtree *stree,Uint slink);

Uint getlargelinkconstruction(Suffixtree *stree);


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

#define ROOT(ST)            ((ST)->branchtab)

// Is the location the root?
#define ROOTLOCATION(LOC)\
        (((LOC)->locstring.length == 0) ? True : False)

// Index of a branch and leaf relative to the first address
#define BRADDR2NUM(ST,A)      ((Uint) ((A) - ROOT(ST)))
#define LEAFADDR2NUM(ST,A)    ((Uint) ((A) - (ST)->leaftab))

// For each branching node we store five integers. These can be accessed by
// some or-combination.
#define ACCESSDEPTH          UintConst(1)
#define ACCESSHEADPOS        (UintConst(1) << 1)
#define ACCESSSUFFIXLINK     (UintConst(1) << 2)
#define ACCESSFIRSTCHILD     (UintConst(1) << 3)
#define ACCESSBRANCHBROTHER  (UintConst(1) << 4)

#ifdef DEBUG
#define SHOWVAL(S)    fprintf(stderr,"#%s %lu\n",#S,(Ulong) S)
#define SETVAL(E,VAL) *(E) = VAL;\
                      if((E) > stree->maxset)\
                      {\
                        stree->maxset = E;\
                      }
#else

#define SETVAL(E,VAL) *(E) = VAL

#endif

// Retrieves the depth and headposition of the branching vertex PT.
#define GETBOTH(DP,HP,PT) \
        if(stree->chainstart != NULL && (PT) >= stree->chainstart)\
        {\
          distance = 1 + \
                     DIVBYSMALLINTS((Uint) (stree->nextfreebranch - (PT)));\
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
          distance = 1 + DIVBYSMALLINTS((Uint) (stree->nextfreebranch - (PT)));\
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
          distance = 1 + DIVBYSMALLINTS((Uint) (stree->nextfreebranch - (PT)));\
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
          stree->headnode = stree->branchtab + GETSUFFIXLINK(stree->headnode);\
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

#ifdef DEBUG
#define SHOWINDEX(NODE)\
        if((NODE) == UNDEFINEDREFERENCE)\
        {\
          DEBUG0(LEADLEVEL,"UNDEFINEDREFERENCE");\
        } else\
        {\
          if(NILPTR(NODE))\
          {\
            DEBUG0(LEADLEVEL,"NILPTR");\
          } else\
          {\
            if(ISLEAF(NODE))\
            {\
              DEBUG1(LEADLEVEL,"Leaf %lu",(Ulong) GETLEAFINDEX(NODE));\
            } else\
            {\
              if(ISLARGE(stree->branchtab[GETBRANCHINDEX(NODE)]))\
              {\
                DEBUG1(LEADLEVEL,"Large %lu",(Ulong) GETBRANCHINDEX(NODE));\
              } else\
              {\
                DEBUG1(LEADLEVEL,"Small %lu",(Ulong) NODE);\
              }\
            }\
          }\
        }
#else
#define SHOWINDEX(NODE) /* Nothing */
#endif

#ifdef DEBUG
void showtable(Suffixtree *stree,Bool final);
void checkstree(Suffixtree *stree);
void showstate(Suffixtree *stree);
void showstree(Suffixtree *stree);
void enumlocations(Suffixtree *stree,void(*processloc)(Suffixtree *stree,Location *));
void checklocation(Suffixtree *stree,Location *loc);
#endif

#ifdef DEBUG
#define CHECKADDR(ST,A)\
        if((A).toleaf)\
        {\
          if(LEAFADDR2NUM(ST,(A).address) > (ST)->textlen)\
          {\
            printf("%s,%lu:",__FILE__,(Ulong) __LINE__);\
            printf("leafaddr = %lu invalid\n",\
                    (Ulong) LEAFADDR2NUM(ST,(A).address));\
            exit(EXIT_FAILURE);\
          }\
        } else\
        {\
          if(BRADDR2NUM(ST,(A).address) >= (ST)->nextfreebranchnum)\
          {\
            printf("%s,%lu:",__FILE__,(Ulong) __LINE__);\
            printf("branchaddr = %lu invalid\n",\
                    (Ulong) BRADDR2NUM(ST,(A).address));\
            exit(EXIT_FAILURE);\
          }\
        }
#else
#define CHECKADDR(ST,A) /* Nothing */
#endif

#endif

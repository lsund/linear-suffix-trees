/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef STREEACC_H
#define STREEACC_H

#include "streelarge.h"

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

#if defined(STREELARGE) || defined(STREESMALL)
#define RECALLSUCC(S)             stree->onsuccpath = S
#else
#define RECALLSUCC(S)             /* Nothing */
#endif

// Set the address for a nil-reference. In the case the reference is a new
// leaf, this is marked
#define RECALLNEWLEAFADDRESS(A)   stree->setlink = A;\
                                  stree->setatnewleaf = True
#define RECALLLEAFADDRESS(A)      stree->setlink = A;\
                                  stree->setatnewleaf = False
#define RECALLBRANCHADDRESS(A)    stree->setlink = (A) + 1;\
                                  stree->setatnewleaf = False

#ifdef STREEHUGE
#define SETNILBIT                 *(stree->setlink) = NILBIT
#else
#define SETNILBIT                 if(stree->setatnewleaf)\
                                  {\
                                    *(stree->setlink) = NILBIT;\
                                  } else\
                                  {\
                                    *(stree->setlink) |= NILBIT;\
                                  }
#endif

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

Sint constructstree(Suffixtree *stree,SYMBOL *text,Uint textlen);

#endif

//}

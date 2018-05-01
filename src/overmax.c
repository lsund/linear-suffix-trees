/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include "bitvector.h"
#include "stree.h"

void overallstree(Suffixtree *stree,Bool skiproot,
                  void(*processnode)(Suffixtree *,Bref,Uint,Uint,void *),
                  void *info)
{
  Uint depth, headposition, *btptr, *largeptr, distance;

  if(skiproot)
  {
    btptr = stree->branchtab + LARGEINTS;
  } else
  {
    btptr = stree->branchtab;
  }
  while(btptr < stree->nextfreebranch)
  {
    if(ISLARGE(*btptr))
    {
      depth = GETDEPTH(btptr);
      headposition = GETHEADPOS(btptr);
      processnode(stree,btptr,depth,headposition,info);
      btptr += LARGEINTS;
    } else
    {
      distance = GETDISTANCE(btptr);
      GETCHAINEND(largeptr,btptr,distance);
      depth = GETDEPTH(largeptr);
      headposition = GETHEADPOS(largeptr);
      while(distance > 0)
      {
        processnode(stree,btptr,depth + distance,headposition - distance,info);
        distance--;
        btptr += SMALLINTS;
      }
      processnode(stree,btptr,depth,headposition,info);
      btptr += LARGEINTS;
    }
  }
}

void overmaximalstree(Suffixtree *stree,
                      void(*processnode)(Suffixtree *,Bref,Uint,Uint,void *),
                      void *info)
{
  Uint *btptr, *nextptr, *largeptr, depth, headposition, distance;

  btptr = stree->branchtab + LARGEINTS; // skip the root
  if(stree->nonmaximal == NULL)
  {
    fprintf(stderr,"stree->nonmaximal is NULL\n");
    exit(EXIT_FAILURE);
  }
  while(btptr < stree->nextfreebranch)
  {
    if(ISLARGE(*btptr))
    {
      depth = GETDEPTH(btptr);
      headposition = GETHEADPOS(btptr);
      nextptr = btptr + LARGEINTS;
    } else
    {
      distance = GETDISTANCE(btptr);
      GETCHAINEND(largeptr,btptr,distance);
      depth = GETDEPTH(largeptr) + distance;
      headposition = GETHEADPOS(largeptr) - distance;
      nextptr = largeptr + LARGEINTS;
    }
    if(!ISIBITSET(stree->nonmaximal,headposition))
    {
      processnode(stree,btptr,depth,headposition,info);
    }
    btptr = nextptr;
  }
}

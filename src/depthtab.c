/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include <stdio.h>
#include <sys/types.h>
#include "debug.h"
#include "types.h"
#include "stree.h"
#include "spaceman.h"

#define ADDAMOUNT 128

void showdepthtab(ArrayUint *dt)
{
  Uint i;

  for(i=0; i<dt->nextfreeUint; i++)
  {
    if(dt->spaceUint[i] > 0)
    {
      printf("Depth %lu %lu\n",(Ulong) i,(Ulong) dt->spaceUint[i]);
    }
  }
}

static void setdepthtab(ArrayUint *depthtab,Uint depth)
{
  Uint i;

  if(depth >= depthtab->allocatedUint)
  {
    depthtab->spaceUint
      = ALLOC(depthtab->spaceUint,Uint,depth+ADDAMOUNT);
    for(i= depthtab->allocatedUint; i<depth+ADDAMOUNT; i++)
    {
      depthtab->spaceUint[i] = 0;
    }
    depthtab->allocatedUint = depth+ADDAMOUNT;
  }
  if(depth + 1 > depthtab->nextfreeUint)
  {
    depthtab->nextfreeUint = depth+1;
  }
  depthtab->spaceUint[depth]++;
}

void makedepthtabstree(ArrayUint *depthtab,Suffixtree *stree)
{
  Uint depth, *btptr, *largeptr, distance;
  //  Uint headposition;

  btptr = stree->branchtab;
  while(btptr < stree->nextfreebranch)
  {
    if(ISLARGE(*btptr))
    {
      depth = GETDEPTH(btptr);
      /* headposition = GETHEADPOS(btptr); */
      setdepthtab(depthtab,depth);
      btptr += LARGEINTS;
    } else
    {
      distance = GETDISTANCE(btptr);
      GETCHAINEND(largeptr,btptr,distance);
      depth = GETDEPTH(largeptr);
      /* headposition = GETHEADPOS(largeptr); */
      while(distance > 0)
      {
        setdepthtab(depthtab,depth + distance);
        distance--;
        btptr += SMALLINTS;
      }
      setdepthtab(depthtab,depth);
      btptr += LARGEINTS;
    }
  }
}

/*
   Copyright (c) 2003 by Stefan Kurtz and The Institute for
   Genomic Research.  This is OSI Certified Open Source Software.
   Please see the file LICENSE for licensing information and
   the file ACKNOWLEDGEMENTS for names of contributors to the
   code base.
   */

#include "arraydef.h"
#include "spaceman.h"
#include "debug.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"

#define SETCURRENT(V)\
    if(ISLEAF(V))\
{\
    currentnode.address = stree->leaftab + GETLEAFINDEX(V);\
    currentnode.toleaf = True;\
} else\
{\
    currentnode.address = stree->branchtab + GETBRANCHINDEX(V);\
    currentnode.toleaf = False;\
}

Sint depthfirststree(Suffixtree *stree,Reference *startnode,
        Sint (*processleaf)(Uint,Bref,void *),
        Bool (*processbranch1)(Bref,void *),
        Sint (*processbranch2)(Bref,void *),
        Bool (*stoptraversal)(void *),void *stopinfo,void *info
    )
{
    Bool godown = True;
    Bool readyforpop = False;
    Uint child;
    Uint brotherval;
    Bref lcpnode = NULL;
    Reference currentnode;
    ArrayBref stack;

    if(startnode->toleaf)
    {
        if(processleaf((Uint) (startnode->address - stree->leaftab),NULL,info) != 0)
        {
            return -1;
        }
        return 0;
    }
    if(stoptraversal != NULL && stoptraversal(stopinfo))
    {
        return 0;
    }

    currentnode.toleaf = False;
    currentnode.address = startnode->address;
    INITARRAY(&stack,Bref);
    STOREINARRAY(&stack,Bref,128,currentnode.address);
    SETCURRENT(GETCHILD(currentnode.address));

    if(processbranch1 == NULL)
    {
#define PROCESSBRANCH1(A,B) /* Nothing */
#define PROCESSBRANCH2(A,B) godown = True
/* #include "dfs.gen" */
while(True)
{
  if(stoptraversal != NULL && stoptraversal(stopinfo))
  {
    return 0;
  }
  if(currentnode.toleaf)
  {
    DEBUG1(3,"visit leaf %lu ",
              (Ulong) LEAFADDR2NUM(stree,currentnode.address));
    DEBUG1(3,"below %lu\n",(Ulong) BRADDR2NUM(stree,startnode->address));
    if(processleaf(LEAFADDR2NUM(stree,currentnode.address),lcpnode,info) != 0)
    {
      return -1;
    }
    brotherval = LEAFBROTHERVAL(*(currentnode.address));
    if(NILPTR(brotherval))
    {
      readyforpop = True;
      currentnode.toleaf = False;
    } else
    {
      SETCURRENT(brotherval);     // current comes from brother
      lcpnode = stack.spaceBref[stack.nextfreeBref-1];
    }
  } else
  {
    if(readyforpop)
    {
      if(stack.nextfreeBref == UintConst(1))
      {
        break;
      }
      (stack.nextfreeBref)--;
      DEBUG1(3,"#pop[%lu]=",(Ulong) stack.nextfreeBref);
      DEBUG1(3,"%lu\n",
             (Ulong) BRADDR2NUM(stree,stack.spaceBref[stack.nextfreeBref]));
      PROCESSBRANCH2(stack.spaceBref[stack.nextfreeBref],info);
      brotherval = GETBROTHER(stack.spaceBref[stack.nextfreeBref]);
      if(!NILPTR(brotherval))
      {
        SETCURRENT(brotherval);    // current comes from brother
        lcpnode = stack.spaceBref[stack.nextfreeBref-1];
        readyforpop = False;
      }
    } else
    {
      DEBUG1(3,"#process1 %lu\n",
               (Ulong) BRADDR2NUM(stree,currentnode.address));
      PROCESSBRANCH1(currentnode.address,info);
      if(godown)
      {
        STOREINARRAY(&stack,Bref,128,currentnode.address);
        DEBUG1(3,"#push[%lu]=",(Ulong) (stack.nextfreeBref-1));
        DEBUG1(3,"%lu\n",(Ulong) BRADDR2NUM(stree,currentnode.address));
        child = GETCHILD(currentnode.address);
        SETCURRENT(child);    // current comes from child
      } else
      {
        brotherval = GETBROTHER(currentnode.address);
        if(NILPTR(brotherval))
        {
          readyforpop = True;
        } else
        {
          SETCURRENT(brotherval);    // current comes brother
        }
      }
    }
  }
}
    } else
    {
#undef PROCESSBRANCH1
#undef PROCESSBRANCH2
#define PROCESSBRANCH1(A,B) godown = processbranch1(A,B)
#define PROCESSBRANCH2(A,B) if(processbranch2(A,B) != 0)\
        {\
            return -2;\
        }
/* #include "dfs.gen" */
while(True)
{
  if(stoptraversal != NULL && stoptraversal(stopinfo))
  {
    return 0;
  }
  if(currentnode.toleaf)
  {
    DEBUG1(3,"visit leaf %lu ",
              (Ulong) LEAFADDR2NUM(stree,currentnode.address));
    DEBUG1(3,"below %lu\n",(Ulong) BRADDR2NUM(stree,startnode->address));
    if(processleaf(LEAFADDR2NUM(stree,currentnode.address),lcpnode,info) != 0)
    {
      return -1;
    }
    brotherval = LEAFBROTHERVAL(*(currentnode.address));
    if(NILPTR(brotherval))
    {
      readyforpop = True;
      currentnode.toleaf = False;
    } else
    {
      SETCURRENT(brotherval);     // current comes from brother
      lcpnode = stack.spaceBref[stack.nextfreeBref-1];
    }
  } else
  {
    if(readyforpop)
    {
      if(stack.nextfreeBref == UintConst(1))
      {
        break;
      }
      (stack.nextfreeBref)--;
      DEBUG1(3,"#pop[%lu]=",(Ulong) stack.nextfreeBref);
      DEBUG1(3,"%lu\n",
             (Ulong) BRADDR2NUM(stree,stack.spaceBref[stack.nextfreeBref]));
      PROCESSBRANCH2(stack.spaceBref[stack.nextfreeBref],info);
      brotherval = GETBROTHER(stack.spaceBref[stack.nextfreeBref]);
      if(!NILPTR(brotherval))
      {
        SETCURRENT(brotherval);    // current comes from brother
        lcpnode = stack.spaceBref[stack.nextfreeBref-1];
        readyforpop = False;
      }
    } else
    {
      DEBUG1(3,"#process1 %lu\n",
               (Ulong) BRADDR2NUM(stree,currentnode.address));
      PROCESSBRANCH1(currentnode.address,info);
      if(godown)
      {
        STOREINARRAY(&stack,Bref,128,currentnode.address);
        DEBUG1(3,"#push[%lu]=",(Ulong) (stack.nextfreeBref-1));
        DEBUG1(3,"%lu\n",(Ulong) BRADDR2NUM(stree,currentnode.address));
        child = GETCHILD(currentnode.address);
        SETCURRENT(child);    // current comes from child
      } else
      {
        brotherval = GETBROTHER(currentnode.address);
        if(NILPTR(brotherval))
        {
          readyforpop = True;
        } else
        {
          SETCURRENT(brotherval);    // current comes brother
        }
      }
    }
  }
}
    }
    FREEARRAY(&stack,Bref);
    return 0;
}

static Sint insertinleaflist(Uint leafindex,/*@unused@*/ Bref lcpnode,
        void *info)
{
    ArrayUint *leaflist = (ArrayUint *) info;

    DEBUG1(3,"insertinleaflist %lu\n",(Ulong) leafindex);
    CHECKARRAYSPACE(leaflist,Uint,256);
    leaflist->spaceUint[leaflist->nextfreeUint++] = leafindex;
    return 0;
}

Sint makeleaflist(Suffixtree *stree,ArrayUint *leaflist,Reference *start)
{
    if(depthfirststree(stree,start,insertinleaflist,NULL,NULL,
                NULL,NULL,(void *) leaflist) != 0)
    {
        return -1;
    }
    return 0;
}

/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include "arraydef.h"
#include "spaceman.h"
#include "stree.h"


typedef struct reference {
    Bool toleaf;
    Uint *address;
} Reference;

typedef struct
{
  Uint headposition,        // the head position of the branching node
       depth;               // the depth of the branching node
  VertexP suffixlink;          // the suffix link is always to a branching node
  Reference firstchild,     // the reference to the first child
            branchbrother;  // the reference to the right brother;
                            // if this doesn't exist then it's \texttt{NULL}
} Branchinfo;               // \Typedef{Branchinfo}

typedef struct
{
  STree *stree;                      // suffix tree info
  ArrayUint countstack;
} Countstate;

static Sint processleaf(/*@unused@*/ Uint leafnumber,
                        /*@unused@*/ VertexP lcpnode,void *info)
{
    leafnumber = 0;
    lcpnode = NULL;

  Countstate *cstate = (Countstate *) info;

  cstate->countstack.spaceUint[cstate->countstack.nextfreeUint - 1]++;
  return 0;
}

#define ACCESSDEPTH          UintConst(1)
#define ACCESSHEADPOS        (UintConst(1) << 1)
#define ACCESSSUFFIXLINK     (UintConst(1) << 2)
#define ACCESSFIRSTCHILD     (UintConst(1) << 3)
#define ACCESSBRANCHBROTHER  (UintConst(1) << 4)

static void int2ref(STree *stree, Reference *ref,Uint i)
{
  if(IS_LEAF(i))
  {
    ref->toleaf = True;
    ref->address = stree->leaves.first + INDEX(i);
  } else
  {
    ref->toleaf = False;
    ref->address = stree->inner.first + INDEX(i);
  }
}

Uint getlargelinkstree(/*@unused@*/ STree *stree, VertexP btptr,Uint depth)
{
    stree = NULL;
  if(depth == UintConst(1))
  {
    return 0;
  }
  return *(btptr+4);
}

void getbranchinfostree(STree *stree,Uint whichinfo,
                        Branchinfo *branchinfo,VertexP btptr)
{
  Uint which = whichinfo, node, distance, *largeptr;

  if(which & ACCESSSUFFIXLINK)
  {
    which |= ACCESSDEPTH;
  }
  if(which & (ACCESSDEPTH | ACCESSHEADPOS))
  {
    if(stree->chain.first != NULL && btptr >= stree->chain.first)
    {
      distance = ((Uint) (stree->inner.next - btptr)) / SMALL_VERTEXSIZE;
      branchinfo->depth = stree->currentdepth + distance;
      branchinfo->headposition = stree->leaves.next_ind - distance;
    } else
    {
      if(IS_LARGE(*btptr))
      {
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = DEPTH(btptr);
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = HEADPOS(btptr);
        }
      } else
      {
        distance = DISTANCE(btptr);
        largeptr = CHAIN_END(btptr,distance);
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = DEPTH(largeptr) + distance;
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = HEADPOS(largeptr) - distance;
        }
      }
    }
  }
  if(which & ACCESSSUFFIXLINK)
  {
    if((stree->chain.first != NULL && btptr >= stree->chain.first) ||
       !IS_LARGE(*btptr))
    {
      branchinfo->suffixlink = btptr + SMALL_VERTEXSIZE;
    } else
    {
      branchinfo->suffixlink = stree->inner.first +
                               getlargelinkstree(stree,btptr,
                                                 branchinfo->depth);
    }
  }
  if(which & ACCESSFIRSTCHILD)
  {
    int2ref(stree,&(branchinfo->firstchild),CHILD(btptr));
  }
  if(which & ACCESSBRANCHBROTHER)
  {
    node = SIBLING(btptr);
    if(IS_NOTHING(node))
    {
      branchinfo->branchbrother.address = NULL;
    } else
    {
      int2ref(stree,&(branchinfo->branchbrother),node);
    }
  }
}

static Bool processbranch1(/*@unused@*/ VertexP nodeptr,void *info)
{
    nodeptr = NULL;

    Countstate *cstate = (Countstate *) info;

  CHECKARRAYSPACE(&(cstate->countstack),Uint,128);
  cstate->countstack.spaceUint[cstate->countstack.nextfreeUint++] = 0;
  return True;
}

#ifdef REPNUM
static Uint tmpleafcount, repnum = 0;
#endif

static Sint processbranch2(VertexP nodeptr,void *info)
{
  Countstate *cstate = (Countstate *) info;
  Branchinfo branchinfo;
  Uint *father;

  cstate->countstack.nextfreeUint--;
  father = cstate->countstack.spaceUint + cstate->countstack.nextfreeUint - 1;
  *father += *(father+1);
  getbranchinfostree(cstate->stree,ACCESSDEPTH | ACCESSHEADPOS,
                     &branchinfo,nodeptr);
#ifdef REPNUM
  if(branchinfo.depth >= 12)
  {
    tmpleafcount = *(father+1);
    repnum += ((tmpleafcount * tmpleafcount) >> 1);
  }
#endif
  cstate->stree->leafcounts[branchinfo.headposition] = *(father+1);
  fprintf(stderr,"leafcount(%lu) = %lu\n",
            (Uint) (nodeptr - cstate->stree->inner.first),
            (Uint) *(father+1));
  *(father+1) = 0;
  return 0;
}

typedef Uint *Bref;

#define DECLAREARRAYSTRUCT(TYPE)\
        typedef struct\
        {\
          TYPE *space##TYPE;\
          Uint allocated##TYPE, nextfree##TYPE;\
        } Array##TYPE

DECLAREARRAYSTRUCT(Bref);

#define SETCURRENT(V)\
        if(IS_LEAF(V))\
        {\
          currentnode.address = stree->leaves.first + INDEX(V);\
          currentnode.toleaf = True;\
        } else\
        {\
          currentnode.address = stree->inner.first + INDEX(V);\
          currentnode.toleaf = False;\
        }

Sint depthfirststree(STree *stree, Reference *startnode,
                     Sint (*processleaf)(Uint,Bref,void *),
                     Bool (*processbranch1)(Bref,void *),
                     Sint (*processbranch2)(Bref,void *),
                     Bool (*stoptraversal)(void *),void *stopinfo,void *info)
{
  stree->leafcounts = ALLOC(NULL,Uint,stree->leaves.next_ind+1);
  Bool godown = True, readyforpop = False;
  Uint child, brotherval;
  Bref lcpnode = NULL;
  Reference currentnode;
  ArrayBref stack;

  if(startnode->toleaf)
  {
    if(processleaf((Uint) (startnode->address - stree->leaves.first), NULL, info) != 0)
    {
      return -1;
    }
    return 0;
  }
  if(stoptraversal != NULL && stoptraversal(stopinfo)) {
    return 0;
  }

  currentnode.toleaf = False;
  currentnode.address = startnode->address;
  INITARRAY(&stack,Bref);
  STOREINARRAY(&stack,Bref,128,currentnode.address);
  SETCURRENT(CHILD(currentnode.address));

  if(processbranch1 == NULL)
  {
#define PROCESSBRANCH1(A,B) /* Nothing */
#define PROCESSBRANCH2(A,B) godown = True
while(True)
{
  if(stoptraversal != NULL && stoptraversal(stopinfo))
  {
    return 0;
  }
  if(currentnode.toleaf)
  {
      fprintf(stderr, "visit leaf %lu ",
              (Uint) LEAFREF_TO_INDEX(stree,currentnode.address));
    fprintf(stderr,"below %lu\n",(Uint) REF_TO_INDEX(startnode->address));
    if(processleaf(LEAFREF_TO_INDEX(stree,currentnode.address),lcpnode,info) != 0)
    {
      return -1;
    }
    brotherval = (*(currentnode.address));
    if(IS_NOTHING(brotherval))
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
      fprintf(stderr,"#pop[%lu]=",(Uint) stack.nextfreeBref);
      fprintf(stderr,"%lu\n",
             (Uint) REF_TO_INDEX(stack.spaceBref[stack.nextfreeBref]));
      PROCESSBRANCH2(stack.spaceBref[stack.nextfreeBref],info);
      brotherval = SIBLING(stack.spaceBref[stack.nextfreeBref]);
      if(!IS_NOTHING(brotherval))
      {
        SETCURRENT(brotherval);    // current comes from brother
        lcpnode = stack.spaceBref[stack.nextfreeBref-1];
        readyforpop = False;
      }
    } else
    {
      fprintf(stderr,"#process1 %lu\n",
               (Uint) REF_TO_INDEX(currentnode.address));
      PROCESSBRANCH1(currentnode.address,info);
      if(godown)
      {
        STOREINARRAY(&stack,Bref,128,currentnode.address);
        fprintf(stderr,"#push[%lu]=",(Uint) (stack.nextfreeBref-1));
        fprintf(stderr,"%lu\n",(Uint) REF_TO_INDEX(currentnode.address));
        child = CHILD(currentnode.address);
        SETCURRENT(child);    // current comes from child
      } else
      {
        brotherval = SIBLING(currentnode.address);
        if(IS_NOTHING(brotherval))
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
while(True)
{
  if(stoptraversal != NULL && stoptraversal(stopinfo))
  {
    return 0;
  }
  if(currentnode.toleaf)
  {
    fprintf(stderr,"visit leaf %lu ",
              (Uint) LEAFREF_TO_INDEX(stree,currentnode.address));
    fprintf(stderr,"below %lu\n",(Uint) REF_TO_INDEX(startnode->address));
    if(processleaf(LEAFREF_TO_INDEX(stree,currentnode.address),lcpnode,info) != 0)
    {
      return -1;
    }
    brotherval = (*(currentnode.address));
    if(IS_NOTHING(brotherval))
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
      fprintf(stderr,"#pop[%lu]=",(Uint) stack.nextfreeBref);
      fprintf(stderr,"%lu\n",
             (Uint) REF_TO_INDEX(stack.spaceBref[stack.nextfreeBref]));
      PROCESSBRANCH2(stack.spaceBref[stack.nextfreeBref],info);
      brotherval = SIBLING(stack.spaceBref[stack.nextfreeBref]);
      if(!IS_NOTHING(brotherval))
      {
        SETCURRENT(brotherval);    // current comes from brother
        lcpnode = stack.spaceBref[stack.nextfreeBref-1];
        readyforpop = False;
      }
    } else
    {
      fprintf(stderr,"#process1 %lu\n",
               (Uint) REF_TO_INDEX(currentnode.address));
      PROCESSBRANCH1(currentnode.address,info);
      if(godown)
      {
        STOREINARRAY(&stack,Bref,128,currentnode.address);
        fprintf(stderr,"#push[%lu]=",(Uint) (stack.nextfreeBref-1));
        fprintf(stderr,"%lu\n",(Uint) REF_TO_INDEX(currentnode.address));
        child = CHILD(currentnode.address);
        SETCURRENT(child);    // current comes from child
      } else
      {
        brotherval = SIBLING(currentnode.address);
        if(IS_NOTHING(brotherval))
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
    lcpnode = NULL;
  ArrayUint *leaflist = (ArrayUint *) info;

  fprintf(stderr,"insertinleaflist %lu\n",(Uint) leafindex);
  CHECKARRAYSPACE(leaflist,Uint,256);
  leaflist->spaceUint[leaflist->nextfreeUint++] = leafindex;
  return 0;
}

Sint makeleaflist(STree *stree,ArrayUint *leaflist,Reference *start)
{
  if(depthfirststree(stree,start,insertinleaflist,NULL,NULL,
                     NULL,NULL,(void *) leaflist) != 0)
  {
    return -1;
  }
  return 0;
}


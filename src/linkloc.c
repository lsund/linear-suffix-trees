/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include "stree.h"

static Uint getlargelinkstree(/*@unused@*/ Suffixtree *stree,Bref btptr,Uint depth)
{
  if(depth == UintConst(1))
  {
    return 0;
  }
  return *(btptr+4);
}

static void int2ref(Suffixtree *stree,Reference *ref,Uint i)
{
  if(ISLEAF(i))
  {
    ref->toleaf = True;
    ref->address = stree->leaftab + GETLEAFINDEX(i);
  } else
  {
    ref->toleaf = False;
    ref->address = stree->branchtab + GETBRANCHINDEX(i);
  }
}

static void getbranchinfostree(
        Suffixtree *stree,
        Uint whichinfo,
        Branchinfo *branchinfo,
        Bref btptr
    )
{
  Uint which = whichinfo, node, distance, *largeptr;

  if(which & ACCESSSUFFIXLINK)
  {
    which |= ACCESSDEPTH;
  }
  if(which & (ACCESSDEPTH | ACCESSHEADPOS))
  {
    if(stree->chainstart != NULL && btptr >= stree->chainstart)
    {
      distance = DIVBYSMALLINTS((Uint) (stree->nextfreebranch - btptr));
      branchinfo->depth = stree->currentdepth + distance;
      branchinfo->headposition = stree->nextfreeleafnum - distance;
    } else
    {
      if(ISLARGE(*btptr))
      {
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = GETDEPTH(btptr);
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = GETHEADPOS(btptr);
        }
      } else
      {
        distance = GETDISTANCE(btptr);
        GETCHAINEND(largeptr,btptr,distance);
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = GETDEPTH(largeptr) + distance;
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = GETHEADPOS(largeptr) - distance;
        }
      }
    }
  }
  if(which & ACCESSSUFFIXLINK)
  {
    if((stree->chainstart != NULL && btptr >= stree->chainstart) ||
       !ISLARGE(*btptr))
    {
      branchinfo->suffixlink = btptr + SMALLINTS;
    } else
    {
      branchinfo->suffixlink = stree->branchtab +
                               getlargelinkstree(stree,btptr,
                                                 branchinfo->depth);
    }
  }
  if(which & ACCESSFIRSTCHILD)
  {
    int2ref(stree,&(branchinfo->firstchild),GETCHILD(btptr));
  }
  if(which & ACCESSBRANCHBROTHER)
  {
    node = GETBROTHER(btptr);
    if(NILPTR(node))
    {
      branchinfo->branchbrother.address = NULL;
    } else
    {
      int2ref(stree,&(branchinfo->branchbrother),node);
    }
  }
}

void rescanstree(
        Suffixtree *stree,
        Location *loc,
        Bref btptr,
        wchar_t *left,
        wchar_t *right
    )
{
  Uint *nodeptr, *largeptr = NULL, leafindex, nodedepth,
       node, distance = 0, prefixlen, headposition, tmpnodedepth;
  wchar_t *lptr;

  lptr = left;
  nodeptr = btptr;
  if(nodeptr == stree->branchtab)
  {
    nodedepth = 0;
    headposition = 0;
  } else
  {
    GETBOTH(nodedepth,headposition,nodeptr);
  }
  loc->nextnode.toleaf = False;
  loc->nextnode.address = nodeptr;
  loc->locstring.start = headposition;
  loc->locstring.length = nodedepth;
  loc->remain = 0;
  while(True)
  {
    if(lptr > right)   // check for empty word
    {
      return;
    }
    if(nodeptr == stree->branchtab)  // at the root
    {
      node = stree->rootchildren[(Uint) *lptr];
      prefixlen = (Uint) (right - lptr + 1);
      if(ISLEAF(node))   // stop if successor is leaf
      {
        leafindex = GETLEAFINDEX(node);
        loc->firstptr = stree->text + leafindex;
        loc->previousnode = stree->branchtab;
        loc->edgelen = stree->textlen - leafindex + 1;
        loc->remain = loc->edgelen - prefixlen;
        loc->nextnode.toleaf = True;
        loc->nextnode.address = stree->leaftab + leafindex;
        loc->locstring.start = leafindex;
        loc->locstring.length = prefixlen;
        return;
      }
      nodeptr = stree->branchtab + GETBRANCHINDEX(node);
      GETONLYHEADPOS(headposition,nodeptr);
      loc->firstptr = stree->text + headposition;
    } else
    {
      node = GETCHILD(nodeptr);
      while(True)             // traverse the list of successors
      {
        if(ISLEAF(node))   // successor is leaf
        {
          leafindex = GETLEAFINDEX(node);
          loc->firstptr = stree->text + (nodedepth + leafindex);
          if(*(loc->firstptr) == *lptr)    // correct edge found
          {
            prefixlen = (Uint) (right - lptr + 1);
            loc->previousnode = loc->nextnode.address;
            loc->edgelen = stree->textlen - (nodedepth + leafindex) + 1;
            loc->remain = loc->edgelen - prefixlen;
            loc->nextnode.toleaf = True;
            loc->nextnode.address = stree->leaftab + leafindex;
            loc->locstring.start = leafindex;
            loc->locstring.length = nodedepth + prefixlen;
            return;
          }
          node = LEAFBROTHERVAL(stree->leaftab[leafindex]);
        } else   // successor is branch node
        {
          nodeptr = stree->branchtab + GETBRANCHINDEX(node);
          GETONLYHEADPOS(headposition,nodeptr);
          loc->firstptr = stree->text + (nodedepth + headposition);
          if(*(loc->firstptr) == *lptr) // correct edge found
          {
            /*@innerbreak@*/ break;
          }
          node = GETBROTHER(nodeptr);
        }
      }
    }
    GETONLYDEPTH(tmpnodedepth,nodeptr);     // get info about succ node
    loc->edgelen = tmpnodedepth - nodedepth;
    prefixlen = (Uint) (right - lptr + 1);
    loc->previousnode = loc->nextnode.address;
    loc->nextnode.toleaf = False;
    loc->nextnode.address = nodeptr;
    loc->locstring.start = headposition;
    loc->locstring.length = nodedepth + prefixlen;
    if(loc->edgelen > prefixlen)     // can reach the successor node
    {
      loc->remain = loc->edgelen - prefixlen;
      return;
    }
    if(loc->edgelen == prefixlen)
    {
      loc->remain = 0;
      return;
    }
    lptr += loc->edgelen;
    nodedepth = tmpnodedepth;
  }
}

void linklocstree(Suffixtree *stree,Location *outloc,Location *inloc)
{
  Branchinfo branchinfo;

  if(inloc->remain == 0)
  {
    outloc->remain = 0;
    outloc->nextnode.toleaf = False;
    getbranchinfostree(stree,ACCESSSUFFIXLINK,&branchinfo,
                       inloc->nextnode.address);
    outloc->nextnode.address = branchinfo.suffixlink;
    outloc->locstring.start = inloc->locstring.start + 1;
    outloc->locstring.length = inloc->locstring.length - 1;
  } else
  {
    if(inloc->previousnode == stree->branchtab)
    {
      rescanstree(stree,outloc,stree->branchtab,inloc->firstptr+1,
                  inloc->firstptr + (inloc->edgelen - inloc->remain) - 1);
    } else
    {
      getbranchinfostree(stree,ACCESSSUFFIXLINK,&branchinfo,
                         inloc->previousnode);
      rescanstree(stree,outloc,branchinfo.suffixlink,inloc->firstptr,
             inloc->firstptr + (inloc->edgelen - inloc->remain) - 1);

    }
  }
}



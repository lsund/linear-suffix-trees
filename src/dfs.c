/*
   Copyright (c) 2003 by Stefan Kurtz and The Institute for
   Genomic Research.  This is OSI Certified Open Source Software.
   Please see the file LICENSE for licensing information and
   the file ACKNOWLEDGEMENTS for names of contributors to the
   code base.
   */

#include "dfs.h"

Uint n_leaves = 0;

typedef struct
{
    Uint headposition,        // the head position of the branching node
         depth;               // the depth of the branching node
    VertexP suffixlink;          // the suffix link is always to a branching node
    Reference fstchild,     // the reference to the fst child
              branchbrother;  // the reference to the right brother;
    // if this doesn't exist then it's \texttt{NULL}
} Branchinfo;               // \Typedef{Branchinfo}

typedef struct
{
    STree *st;                      // suffix tree info
    ArrayUint countstack;
} Countstate;

#define ACCESSDEPTH          UintConst(1)
#define ACCESSHEADPOS        (UintConst(1) << 1)
#define ACCESSSUFFIXLINK     (UintConst(1) << 2)
#define ACCESSFIRSTCHILD     (UintConst(1) << 3)
#define ACCESSBRANCHBROTHER  (UintConst(1) << 4)

#define SETCURRENT(V)\
    if(IS_LEAF(V))\
{\
    currentnode.address = st->ls.fst + VERTEX_TO_INDEX(V);\
    currentnode.toleaf = true;\
} else\
{\
    currentnode.address = VERTEX_TO_REF(V);\
    currentnode.toleaf = false;\
}

Sint stree_dfs(STree *st, Reference *start,
        Sint (*processleaf) (Uint, ArrayUint *), ArrayUint *leaves)
{
    bool godown = true, readyforpop = false;
    Uint child, brotherval;
    Bref lcpnode = NULL;
    Reference currentnode;
    ArrayBref stack;

    if(start->toleaf)
    {
        processleaf((Uint) (start->address - st->ls.fst), leaves);
        return 0;
    }

    currentnode.toleaf = false;
    currentnode.address = start->address;
    stack.spaceBref = NULL;
    stack.allocatedBref = stack.nxtfreeBref = 0;
    if (stack.nxtfreeBref >= leaves->allocatedUint) {
        stack.allocatedBref += 256;
        stack.spaceBref =
            realloc(leaves->spaceUint, sizeof(Uint) * leaves->allocatedUint);
    }
    stack.spaceBref[stack.nxtfreeBref++] = currentnode.address;
    SETCURRENT(CHILD(currentnode.address));

        while(true)
        {
            if(currentnode.toleaf) {
                processleaf(LEAFREF_TO_INDEX(currentnode.address), leaves);
                brotherval = LEAF_SIBLING(currentnode.address);
                if(IS_NOTHING(brotherval)) {
                    readyforpop = true;
                    currentnode.toleaf = false;
                } else {
                    SETCURRENT(brotherval);     // current comes from brother
                    lcpnode = stack.spaceBref[stack.nxtfreeBref-1];
                }
            } else {
                if(readyforpop) {
                    if(stack.nxtfreeBref == UintConst(1)) {
                        // IS root?
                        break;
                    }
                    (stack.nxtfreeBref)--;
                    brotherval = SIBLING(stack.spaceBref[stack.nxtfreeBref]);
                    if(!IS_NOTHING(brotherval))
                    {
                        SETCURRENT(brotherval);    // current comes from brother
                        lcpnode = stack.spaceBref[stack.nxtfreeBref-1];
                        readyforpop = false;
                    }
                } else {
                    if(godown) {
                        stack.spaceBref = NULL;
                        stack.allocatedBref = stack.nxtfreeBref = 0;
                        if (stack.nxtfreeBref >= leaves->allocatedUint) {
                            stack.allocatedBref += 256;
                            stack.spaceBref =
                                realloc(leaves->spaceUint, sizeof(Uint) * leaves->allocatedUint);
                        }
                        stack.spaceBref[stack.nxtfreeBref++] = currentnode.address;
                        child = CHILD(currentnode.address);
                        SETCURRENT(child);    // current comes from child
                    } else {
                        brotherval = SIBLING(currentnode.address);
                        if(IS_NOTHING(brotherval)) {
                            readyforpop = true;
                        } else {
                            SETCURRENT(brotherval);    // current comes brother
                        }
                    }
                }
            }
        }
    free(stack.spaceBref);
    return 0;
}

static Sint insertinleaflist(Uint leafindex, ArrayUint *leaves)
{
    n_leaves++;
    /* fprintf(stderr,"insertinleaflist %lu\n",(Uint) leafindex); */
    if (leaves->nxtfreeUint >= leaves->allocatedUint) {
        leaves->allocatedUint += 256;
        leaves->spaceUint =
            realloc(leaves->spaceUint, sizeof(Uint) * leaves->allocatedUint);
    }
    /* CHECKARRAYSPACE(leaves, Uint, 256); */
    leaves->spaceUint[leaves->nxtfreeUint++] = leafindex;

    return 0;
}

Sint makeleaflist(STree *st,ArrayUint *leaves, Reference *start)
{
    if(stree_dfs(st,start,insertinleaflist, leaves) != 0)
    {
        return -1;
    }
    for (Uint i = 0; i < n_leaves; i++) {
        printf("%lu\n", leaves->spaceUint[i]);
    }
    return 0;
}


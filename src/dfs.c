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
    Reference firstchild,     // the reference to the first child
              branchbrother;  // the reference to the right brother;
    // if this doesn't exist then it's \texttt{NULL}
} Branchinfo;               // \Typedef{Branchinfo}

typedef struct
{
    STree *stree;                      // suffix tree info
    ArrayUint countstack;
} Countstate;

#define ACCESSDEPTH          UintConst(1)
#define ACCESSHEADPOS        (UintConst(1) << 1)
#define ACCESSSUFFIXLINK     (UintConst(1) << 2)
#define ACCESSFIRSTCHILD     (UintConst(1) << 3)
#define ACCESSBRANCHBROTHER  (UintConst(1) << 4)

typedef Uint *Bref;

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

Sint stree_dfs(STree *stree, Reference *start,
        Sint (*processleaf) (Uint, ArrayUint *), ArrayUint *leaves)
{
    stree->leafcounts = ALLOC(NULL, Uint, stree->leaves.next_ind + 1);
    Bool godown = True, readyforpop = False;
    Uint child, brotherval;
    Bref lcpnode = NULL;
    Reference currentnode;
    ArrayBref stack;

    if(start->toleaf)
    {
        processleaf((Uint) (start->address - stree->leaves.first), leaves);
        return 0;
    }

    currentnode.toleaf = False;
    currentnode.address = start->address;
    INITARRAY(&stack,Bref);
    STOREINARRAY(&stack,Bref,128,currentnode.address);
    SETCURRENT(CHILD(currentnode.address));

        while(True)
        {
            if(currentnode.toleaf) {
                /* fprintf(stderr, "visit leaf %lu ", */
                /*         (Uint) LEAFREF_TO_INDEX(stree, currentnode.address)); */
                /* fprintf(stderr,"below %lu\n",(Uint) REF_TO_INDEX(start->address)); */
                processleaf(LEAFREF_TO_INDEX(stree,currentnode.address), leaves);
                brotherval = LEAF_SIBLING(currentnode.address);
                /* printf("sibling: %lu\n", INDEX(brotherval)); */
                if(IS_NOTHING(brotherval)) {
                    readyforpop = True;
                    currentnode.toleaf = False;
                } else {
                    SETCURRENT(brotherval);     // current comes from brother
                    lcpnode = stack.spaceBref[stack.nextfreeBref-1];
                }
            } else {
                if(readyforpop) {
                    if(stack.nextfreeBref == UintConst(1)) {
                        // IS root?
                        break;
                    }
                    (stack.nextfreeBref)--;
                    /* fprintf(stderr,"#pop[%lu]=",(Uint) stack.nextfreeBref); */
                    /* fprintf(stderr,"%lu\n", */
                    /*         (Uint) REF_TO_INDEX(stack.spaceBref[stack.nextfreeBref])); */
                    brotherval = SIBLING(stack.spaceBref[stack.nextfreeBref]);
                    if(!IS_NOTHING(brotherval))
                    {
                        SETCURRENT(brotherval);    // current comes from brother
                        lcpnode = stack.spaceBref[stack.nextfreeBref-1];
                        readyforpop = False;
                    }
                } else {
                    /* fprintf(stderr,"#process1 %lu\n", */
                    /*         (Uint) REF_TO_INDEX(currentnode.address)); */
                    if(godown) {
                        STOREINARRAY(&stack,Bref,128,currentnode.address);
                        /* fprintf(stderr,"#push[%lu]=",(Uint) (stack.nextfreeBref-1)); */
                        /* fprintf(stderr,"%lu\n",(Uint) REF_TO_INDEX(currentnode.address)); */
                        child = CHILD(currentnode.address);
                        SETCURRENT(child);    // current comes from child
                    } else {
                        brotherval = SIBLING(currentnode.address);
                        if(IS_NOTHING(brotherval)) {
                            readyforpop = True;
                        } else {
                            SETCURRENT(brotherval);    // current comes brother
                        }
                    }
                }
            }
        }
    FREEARRAY(&stack,Bref);
    return 0;
}

static Sint insertinleaflist(Uint leafindex, ArrayUint *leaves)
{
    n_leaves++;
    /* fprintf(stderr,"insertinleaflist %lu\n",(Uint) leafindex); */
    CHECKARRAYSPACE(leaves, Uint, 256);
    leaves->spaceUint[leaves->nextfreeUint++] = leafindex;

    return 0;
}

Sint makeleaflist(STree *stree,ArrayUint *leaves, Reference *start)
{
    if(stree_dfs(stree,start,insertinleaflist, leaves) != 0)
    {
        return -1;
    }
    for (Uint i = 0; i < n_leaves; i++) {
        printf("%lu\n", leaves->spaceUint[i]);
    }
    return 0;
}


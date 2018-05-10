/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include "stree.h"

Wchar *text;
Wchar *sentinel;
Uint textlen, alphasize;


static void unset_chain(STree *stree)
{
    stree->chain.size = 0;
    stree->chain.first = NULL;
}


void init_chain(STree *stree)
{
    stree->chain.first = stree->inner.next;
}

void finalize_chain(STree *stree)
{
    Uint distance;

    SUFFIX_LINK(stree->inner.next) = REF_TO_INDEX(stree->head.origin);
    if (stree->chain.size > 0) {
        VertexP prev = stree->inner.next;
        for(distance = 1; distance <= stree->chain.size; distance++) {
            prev -= SMALL_VERTEXSIZE;
            DISTANCE(prev) = distance;
            *prev = WITH_SMALLBIT(*prev);
        }
        unset_chain(stree);
    }
    stree->inner.next += LARGE_VERTEXSIZE;
    stree->inner.next_ind += LARGE_VERTEXSIZE;
}

void linkrootchildren(STree *stree)
{
    Uint *rcptr, *prevnodeptr, prev = UNDEF;

    alphasize = 0;
    for(rcptr = stree->rootchildren;
            rcptr <= stree->rootchildren + MAX_CHARS; rcptr++)
    {
        if(*rcptr != UNDEF)
        {
            alphasize++;
            if(prev == UNDEF)
            {
                SET_CHILD(stree->inner.first, *rcptr);
            } else
            {
                if (IS_LEAF(prev)) {
                    stree->leaves.first[INDEX(prev)] = *rcptr;
                } else {
                    prevnodeptr = stree->inner.first + INDEX(prev);
                    SIBLING(prevnodeptr) = *rcptr;
                }
            }
            prev = *rcptr;
        }
    }
    if(IS_LEAF(prev))
    {
        stree->leaves.first[INDEX(prev)] = WITH_LEAFBIT(textlen);
    } else
    {
        prevnodeptr = stree->inner.first + INDEX(prev);
        SIBLING(prevnodeptr) = WITH_LEAFBIT(textlen);
    }
    stree->leaves.first[textlen] = NOTHING;
}


void init(STree *stree)
{
    Uint i;

    stree->inner.size = START_ALLOCSIZE;
    stree->leaves.first = ALLOC(NULL, Uint, textlen + 2);

    stree->inner.first = ALLOC(NULL, Uint, stree->inner.size);
    for(i=0; i<LARGE_VERTEXSIZE; i++) {
        stree->inner.first[i] = 0;
    }

    stree->rootchildren = ALLOC(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    stree->tailptr = text;

    Uint last = stree->inner.size - LARGE_VERTEXSIZE;
    stree->allocated = stree->inner.first + last;

    // Inner
    stree->inner.next = stree->inner.first;
    stree->inner.next_ind = 0;
    DEPTH(stree->inner.next) = 0;
    HEADPOS(stree->inner.next) = 0;
    SET_CHILD(stree->inner.next, WITH_LEAFBIT(0));
    SIBLING(stree->inner.next) = 0;

    // Head
    stree->head.origin    = stree->inner.first;
    stree->head.label.end = NULL;
    stree->head.depth     = 0;

    SET_ROOTCHILD(*text, WITH_LEAFBIT(0));

    stree->leaves.first[0]            = 0;
    stree->leaves.next_ind            = 1;
    stree->leaves.next                = stree->leaves.first + 1;
    stree->inner.next                 = stree->inner.first + LARGE_VERTEXSIZE;
    stree->inner.next_ind             = LARGE_VERTEXSIZE;
    stree->split_vertex               = UNDEF;
    stree->insertprev                 = UNDEF;
    stree->chain.size               = 0;
    stree->chain.first                 = NULL;

}

void freestree(STree *stree)
{
    FREE(stree->leaves.first);
    FREE(stree->rootchildren);
    FREE(stree->inner.first);
}



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
Uint textlen;


static void unset_chain(STree *stree)
{
    stree->chain.size = 0;
    stree->chain.first = NULL;
}


void init_chain(STree *stree)
{
    stree->chain.first = stree->inner.next;
}


void collapse_chain(STree *stree)
{
    Uint distance;

    SET_SUFFIXLINK(INDEX(stree->headnode));
    if (stree->chain.size > 0) {
        VertexP prev = stree->inner.next;
        for(distance = 1; distance <= stree->chain.size; distance++) {
            prev -= SMALL_VERTEXSIZE;
            SET_DISTANCE(prev, distance);
        }
        unset_chain(stree);
    }
    stree->inner.next += LARGE_VERTEXSIZE;
    stree->inner.next_num += LARGE_VERTEXSIZE;
}

void linkrootchildren(STree *stree)
{
    Uint *rcptr, *prevnodeptr, prev = UNDEF;

    stree->alphasize = 0;
    for(rcptr = stree->rootchildren;
            rcptr <= stree->rootchildren + MAX_CHARS; rcptr++)
    {
        if(*rcptr != UNDEF)
        {
            stree->alphasize++;
            if(prev == UNDEF)
            {
                SET_CHILD(stree->inner.first, MAKE_LARGE(*rcptr));
            } else
            {
                if(IS_LEAF(prev))
                {
                    stree->leaves.first[LEAF_NUMBER(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->inner.first + LEAF_NUMBER(prev);
                    SET_SIBLING(prevnodeptr,*rcptr);
                }
            }
            prev = *rcptr;
        }
    }
    if(IS_LEAF(prev))
    {
        stree->leaves.first[LEAF_NUMBER(prev)] = MAKE_LEAF(textlen);
    } else
    {
        prevnodeptr = stree->inner.first + LEAF_NUMBER(prev);
        SET_SIBLING(prevnodeptr,MAKE_LEAF(textlen));
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
    stree->allocated
        = stree->inner.first + stree->inner.size - LARGE_VERTEXSIZE;
    stree->headnode = stree->inner.next = stree->inner.first;
    stree->head.end = NULL;
    stree->head.depth = stree->maxbranchdepth = 0;

    stree->inner.next = stree->inner.first;
    stree->inner.next_num = 0;

    SET_DEPTH(0);
    SET_HEAD(0);
    SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(0), 0);
    stree->rootchildren[(Uint) *text] = MAKE_LEAF(0); // Necessary?
    stree->leaves.first[0]            = 0;

    stree->leafcounts                 = NULL;
    stree->leaves.next_num            = 1;
    stree->leaves.next                = stree->leaves.first + 1;
    stree->inner.next                 = stree->inner.first + LARGE_VERTEXSIZE;
    stree->inner.next_num             = LARGE_VERTEXSIZE;
    stree->split_vertex               = UNDEF;
    stree->insertprev                 = UNDEF;
    stree->chain.size               = 0;
    stree->chain.first                 = NULL;
    stree->nonmaximal                 = NULL;

}

void freestree(STree *stree)
{
    FREE(stree->leaves.first);
    FREE(stree->rootchildren);
    FREE(stree->inner.first);
    if(stree->nonmaximal != NULL) {
        FREE(stree->nonmaximal);
    }
    if(stree->leafcounts != NULL) {
        FREE(stree->leafcounts);
    }
}



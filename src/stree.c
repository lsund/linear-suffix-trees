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

void reduce_depth(STree *stree)
{
    Uint distance, *backwards;

    if(stree->chain_remain > 0) {
        backwards = stree->inner.next;
        for(distance = 1; distance <= stree->chain_remain; distance++) {
            backwards -= SMALL_WIDTH;
            SET_DISTANCE(backwards, distance);
        }
        stree->chain_remain = 0;
        stree->chainstart = NULL;
    }
    stree->inner.next += LARGE_WIDTH;
    stree->inner.next_num += LARGE_WIDTH;
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
                    stree->leaves.first[LEAF_NUM(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->inner.first + LEAF_NUM(prev);
                    SET_SIBLING(prevnodeptr,*rcptr);
                }
            }
            prev = *rcptr;
        }
    }
    if(IS_LEAF(prev))
    {
        stree->leaves.first[LEAF_NUM(prev)] = MAKE_LEAF(textlen);
    } else
    {
        prevnodeptr = stree->inner.first + LEAF_NUM(prev);
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
    for(i=0; i<LARGE_WIDTH; i++) {
        stree->inner.first[i] = 0;
    }

    stree->rootchildren = ALLOC(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    stree->tailptr = text;
    stree->allocated
        = stree->inner.first + stree->inner.size - LARGE_WIDTH;
    stree->headnode = stree->inner.next = stree->inner.first;
    stree->vertex_succ_head = NULL;
    stree->head_depth = stree->maxbranchdepth = 0;

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
    stree->inner.next                 = stree->inner.first + LARGE_WIDTH;
    stree->inner.next_num             = LARGE_WIDTH;
    stree->split_vertex               = UNDEF;
    stree->insertprev                 = UNDEF;
    stree->chain_remain               = 0;
    stree->chainstart                 = NULL;
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



/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include "stree_aux.h"

Uint textlen;

void update_chain(STree *stree, Uint *vertexp, Uint **chainend, Uint *distance)
{
    if(stree->chain.first != NULL && vertexp >= stree->chain.first) {
        *distance = 1 + (stree->inner.next - vertexp) / SMALL_VERTEXSIZE;
    } else {
        if(IS_SMALL(*(vertexp))) {
            *distance = DISTANCE(vertexp);
            *chainend   = CHAIN_END(vertexp, *distance);
        }
    }
}

Uint get_head(STree *stree, Uint *vertexp, Uint **chainend, Uint distance)
{
    if(stree->chain.first != NULL && vertexp >= stree->chain.first) {
        return stree->leaves.next_ind - distance;
    } else {
        if(IS_LARGE(*(vertexp))) {
            return HEAD(vertexp);
        } else {
            return HEAD(*chainend) - distance;
        }
    }
}

Uint get_depth(STree *stree, Uint *vertexp, Uint distance, Uint **chainend)
{
    if(stree->chain.first != NULL && vertexp >= stree->chain.first) {
        return stree->currentdepth  + distance;
    } else {
        if(IS_LARGE(*vertexp)) {
            return DEPTH(vertexp);
        } else {
            return DEPTH(*chainend) + distance;
        }
    }
}


static Wchar second_headedge_character(STree *stree)
{
    if (stree->headedge.end == NULL) {
        return *(stree->tailptr-1);
    } else {
        return *(stree->tailptr - (stree->headedge.end - stree->headedge.start + 2));
    }
}


static Uint* suffix_link(STree *stree)
{
    Uint *first = stree->inner.first;
    if(LINK_TO_ROOT) {
        return first;
    } else if (LINK_TO_ROOTCHILD) {
        return first + stree->rootchildren[(Uint) second_headedge_character(stree)];
    } else {
        return first + SUFFIX_LINK(stree->headedge.vertex);
    }
}

void follow_link(STree *stree)
{
    if(IS_LARGE(*(stree->headedge.vertex))) {
        stree->headedge.vertex = suffix_link(stree);
    } else {
        stree->headedge.vertex += SMALL_VERTEXSIZE;
    }
    stree->headedge.depth--;
}

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
        return stree->leaves.next_num - distance;
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


static Uint suffix_link(STree *stree)
{
    Wchar secondchar;

    if(stree->head.depth == 1) {
        return 0;        // link refers to root
    }
    if(stree->head.depth == 2) {
        // determine second char of egde
        if(stree->head.end == NULL) {
            secondchar = *(stree->tailptr-1);
        } else {
            secondchar = *(stree->tailptr - (stree->head.end - stree->head.start + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    return SUFFIX_LINK(stree->headnode);
}

void follow_link(STree *stree)
{
    if(IS_LARGE(*(stree->headnode))) {
        stree->headnode = stree->inner.first + suffix_link(stree);
    } else {
        stree->headnode += SMALL_VERTEXSIZE;
    }
    stree->head.depth--;
}

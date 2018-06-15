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


Uint head_label_depth(STree *stree)
{
    return stree->head.depth;
}

Bool base_is_vertex(STree *stree)
{
    return stree->head.label.end == NULL;
}


Bool is_head_old(STree *stree)
{
    return base_is_vertex(stree);
}


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

Uint get_headpos(STree *stree, Uint *vertexp, Uint **chainend, Uint distance)
{
    if(stree->chain.first != NULL && vertexp >= stree->chain.first) {
        return stree->leaves.next_ind - distance;
    } else {
        if(IS_LARGE(*(vertexp))) {
            return HEADPOS(vertexp);
        } else {
            return HEADPOS(*chainend) - distance;
        }
    }
}

Uint get_depth(STree *stree, Uint *vertexp, Uint distance, Uint **chainend)
{
    if(stree->chain.first != NULL && vertexp >= stree->chain.first) {
        return stree->current_branchdepth  + distance;
    } else {
        if(IS_LARGE(*vertexp)) {
            return DEPTH(vertexp);
        } else {
            return DEPTH(*chainend) + distance;
        }
    }
}

static Uint* suffix_link(STree *stree)
{
    Uint *first = stree->inner.first;
    if(stree->head.depth == 1) {
        return first;
    } else {
        return first + SUFFIX_LINK(stree->head.origin);
    }
}

void follow_link(STree *stree)
{
    if(IS_LARGE(*stree->head.origin)) {
        stree->head.origin = suffix_link(stree);
    } else {
        stree->head.origin += SMALL_VERTEXSIZE;
    }
    stree->head.depth--;
}

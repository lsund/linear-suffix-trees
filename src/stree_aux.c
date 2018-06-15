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


Bool base_is_vertex(STree *st)
{
    return st->head.label.end == NULL;
}


Bool is_head_old(STree *st)
{
    return base_is_vertex(st);
}


void update_chain(STree *st, Uint *vertexp, Uint **chainend, Uint *distance)
{
    if(st->chain.first != NULL && vertexp >= st->chain.first) {
        *distance = 1 + (st->inner.next - vertexp) / SMALL_VERTEXSIZE;
    } else {
        if(IS_SMALL(*(vertexp))) {
            *distance = DISTANCE(vertexp);
            *chainend   = CHAIN_END(vertexp, *distance);
        }
    }
}

Uint get_headpos(STree *st, Uint *vertexp, Uint **chainend, Uint distance)
{
    if(st->chain.first != NULL && vertexp >= st->chain.first) {
        return st->leaves.next_ind - distance;
    } else {
        if(IS_LARGE(*(vertexp))) {
            return HEADPOS(vertexp);
        } else {
            return HEADPOS(*chainend) - distance;
        }
    }
}

Uint get_depth(STree *st, Uint *vertexp, Uint distance, Uint **chainend)
{
    if(st->chain.first != NULL && vertexp >= st->chain.first) {
        return st->current_branchdepth  + distance;
    } else {
        if(IS_LARGE(*vertexp)) {
            return DEPTH(vertexp);
        } else {
            return DEPTH(*chainend) + distance;
        }
    }
}

static Uint* suffix_link(STree *st)
{
    Uint *first = st->inner.first;
    if(st->head.depth == 1) {
        return first;
    } else {
        return first + SUFFIX_LINK(st->head.origin);
    }
}

void follow_link(STree *st)
{
    if(IS_LARGE(*st->head.origin)) {
        st->head.origin = suffix_link(st);
    } else {
        st->head.origin += SMALL_VERTEXSIZE;
    }
    st->head.depth--;
}

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

bool base_is_vertex(STree *st)
{
    return st->head.label.end == NULL;
}


bool is_head_old(STree *st)
{
    return base_is_vertex(st);
}


bool tail_at_lastchar(STree *st)
{
    return st->tail == text.sentinel;
}


bool head_is_root(STree *st)
{
    return st->head.depth == 0;
}

void st_free(STree *st)
{
    free(st->ls.fst);
    free(st->rootchildren);
    free(st->is.fst);
}


Uint get_headpos(STree *st, Uint *vertexp, Uint **chainend, Uint distance)
{
    if(st->chain.fst != NULL && vertexp >= st->chain.fst) {
        return st->ls.nxt_ind - distance;
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
    if(st->chain.fst != NULL && vertexp >= st->chain.fst) {
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
    Uint *fst = st->is.fst;
    if(st->head.depth == 1) {
        return fst;
    } else {
        return fst + SUFFIX_LINK(st->head.vertex);
    }
}

void follow_link(STree *st)
{
    if(IS_LARGE(*st->head.vertex)) {
        st->head.vertex = suffix_link(st);
    } else {
        st->head.vertex += SMALL_VERTEXSIZE;
    }
    st->head.depth--;
}

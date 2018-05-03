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

Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep)
{
    Uint distance = 0;
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {

        distance = 1 +
            DIV_SMALL_WIDTH((Uint) (stree->inner.next - (vertexp)));
        *depth = stree->currentdepth + distance;
        *head = stree->leaf_vertices.next_num - distance;

    } else {

        if(IS_LARGE(*(vertexp))) {

            *depth = DEPTH(vertexp);
            *head = HEAD(vertexp);

        } else {

            distance = DISTANCE(vertexp);
            largep = CHAIN_END(vertexp, distance);
            *depth = DEPTH(largep) + distance;
            *head = HEAD(largep) - distance;

        }
    }
    return distance;
}


Uint get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIV_SMALL_WIDTH((Uint) (stree->inner.next - vertexp));
        return stree->leaf_vertices.next_num - *distance;
    } else
    {
        if(IS_LARGE(*(vertexp))) {
            return HEAD(vertexp);
        } else {
            *distance = DISTANCE(vertexp);
            *largep = CHAIN_END(vertexp, *distance);
            return HEAD(*largep) - *distance;
        }
    }
}

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIV_SMALL_WIDTH((Uint) (stree->inner.next - vertexp));
        return stree->currentdepth  + *distance;
    } else {
        if(IS_LARGE(*vertexp)) {
            return DEPTH(vertexp);
        } else {
            *distance = DISTANCE(vertexp);
            *largep = CHAIN_END(vertexp, *distance);
            return DEPTH(*largep) + *distance;
        }
    }
}


Uint get_depth_after_head(STree *stree, Uint *vertexp, Uint *distance, Uint **largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
         return stree->currentdepth + *distance;
    } else {
        if(IS_LARGE(*vertexp)) {
            return DEPTH(vertexp);
        } else {
            return DEPTH(*largep) + *distance;
        }
    }
}

static Uint suffix_link(STree *stree)
{
    Wchar secondchar;

    if(stree->head_depth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->head_depth == 2)  // determine second char of egde
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    return *(stree->headnode+4);
}

void follow_link(STree *stree)
{
    if(IS_LARGE(*(stree->headnode))) {
        stree->headnode = stree->inner.first + suffix_link(stree);
    } else {
        stree->headnode += SMALL_WIDTH;
    }
    stree->head_depth--;
}

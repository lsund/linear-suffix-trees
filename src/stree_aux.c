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
            DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (vertexp)));
        *depth = stree->currentdepth + distance;
        *head = stree->leaf_vertices.next_free_num - distance;

    } else {

        if(ISLARGE(*(vertexp))) {

            *depth = GETDEPTH(vertexp);
            *head = GETHEADPOS(vertexp);

        } else {

            distance = GETDISTANCE(vertexp);
            GETCHAINEND(largep, vertexp, distance);
            *depth = GETDEPTH(largep) + distance;
            *head = GETHEADPOS(largep) - distance;

        }
    }
    return distance;
}


Uint get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        return stree->leaf_vertices.next_free_num - *distance;
    } else
    {
        if(ISLARGE(*(vertexp))) {
            return GETHEADPOS(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(*largep, vertexp, *distance);
            return GETHEADPOS(*largep) - *distance;
        }
    }
}

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        return stree->currentdepth  + *distance;
    } else {
        if(ISLARGE(*vertexp)) {
            return GETDEPTH(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(*largep, vertexp, *distance);
            return GETDEPTH(*largep) + *distance;
        }
    }
}


Uint get_depth_after_head(STree *stree, Uint *vertexp, Uint *distance, Uint **largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
         return stree->currentdepth + *distance;
    } else {
        if(ISLARGE(*vertexp)) {
            return GETDEPTH(vertexp);
        } else {
            return GETDEPTH(*largep) + *distance;
        }
    }
}

static Uint getlargelinkconstruction(STree *stree)
{
    Wchar secondchar;

    if(stree->headnodedepth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->headnodedepth == 2)  // determine second char of egde
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
    if(ISLARGE(*(stree->headnode))) {
        stree->headnode = stree->inner_vertices.first + GETSUFFIXLINK(stree->headnode);
    } else {
        stree->headnode += SMALLINTS;
    }
    stree->headnodedepth--;
}

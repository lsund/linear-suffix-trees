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
#include "stree_aux.h"
#include "insert.h"
#include "skip_count.h"
#include "scan.h"

Wchar *text;
Uint textlen;

///////////////////////////////////////////////////////////////////////////////
// Private


static void insert_vertex(STree *stree)
{
    if(IS_HEAD_A_VERTEX) {
        insert_leaf(stree);
    } else {
        insert_inner(stree);
    }
}


static void increment_chain(STree *stree)
{
    stree->chain_remain   += 1;
    stree->inner.next     += SMALL_WIDTH;
    stree->inner.next_num += SMALL_WIDTH;
}


///////////////////////////////////////////////////////////////////////////////
// Public


Sint construct(STree *stree)
{
    init(stree);

    while(!IS_SENTINEL(stree->tailptr)) {


        // The head at the root vertex, or directly under it
        if(IS_HEAD_ROOT) {

            (stree->tailptr)++;
            walk(stree);

        } else if (IS_HEAD_A_VERTEX) {

            follow_link(stree);
            walk(stree);

        // The head is somewhere else than the root
        } else {

            if(!IS_HEADDEPTH_ZERO) {

                follow_link(stree);
                skip_count(stree);

            } else if (stree->headstart == stree->vertex_succ_head) {
                stree->vertex_succ_head = NULL;
            } else {
                stree->headstart++;
                skip_count(stree);
            }

            // The head is a vertex
            if(IS_HEAD_A_VERTEX) {

                SET_SUFFIXLINK(INDEX(stree->headnode));
                reduce_depth(stree);
                walk(stree);

            // The head is a edge
            } else if (IS_CHAIN_LONG) {

                SET_SUFFIXLINK(stree->inner.next_num + LARGE_WIDTH);
                reduce_depth(stree);

            } else if (IS_CHAIN_UNDEF) {
                stree->chainstart = stree->inner.next;
                increment_chain(stree);
            } else {
                increment_chain(stree);
            }
        }
        insert_vertex(stree);
    }

    stree->chainstart = NULL;
    linkrootchildren(stree);

    return 0;
}

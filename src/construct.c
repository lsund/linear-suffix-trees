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


static void grow_chain(STree *stree)
{
    if (!stree->chain.first) {
        init_chain(stree);
    }
    stree->chain.size     += 1;
    stree->inner.next     += SMALL_VERTEXSIZE;
    stree->inner.next_num += SMALL_VERTEXSIZE;
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

            } else if (stree->head.start == stree->head.end) {
                stree->head.end = NULL;
            } else {
                stree->head.start++;
                skip_count(stree);
            }

            // The head is a vertex, somewhere else than the root
            if(IS_HEAD_A_VERTEX) {
                collapse_chain(stree);
                walk(stree);

            } else {
                grow_chain(stree);
            }
        }
        insert_vertex(stree);
    }

    stree->chain.first = NULL;
    linkrootchildren(stree);

    return 0;
}

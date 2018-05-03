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

Wchar *text;
Uint textlen;

///////////////////////////////////////////////////////////////////////////////
// Private


static void insert_vertex(STree *stree)
{
    if(IS_HEAD_VERTEX) {
        insert_leaf(stree);
    } else {
        insert_inner(stree);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Public


Sint construct(STree *stree)
{
    stree->nonmaximal = NULL;

    init(stree);

    while(!IS_LAST(stree->tailptr) || IS_HEAD_ROOT) {

        if(IS_HEAD_ROOT) {
            // Case 1
            (stree->tailptr)++;
            scanprefix(stree);
        } else {
            // Case 2
            if(IS_HEAD_VERTEX) {
                // Case 2.1: Head is node
                follow_link(stree);
                scanprefix(stree);
            } else {
                // Case 2.2
                if(IS_ROOT_DEPTH) {
                    // Case 2.2.1: at root, do not use links
                    if(IS_HEAD_EMPTY) {
                        // No need to rescan
                        stree->headend = NULL;
                    } else {
                        (stree->headstart)++;
                        rescan(stree);
                    }
                // Case 2.2.2
                } else {
                    follow_link(stree);
                    rescan(stree);
                }
                // Case 2.2.3
                if(IS_HEAD_VERTEX) {

                    SET_SUFFIXLINK(INDEX(stree->headnode));
                    completelarge(stree);
                    scanprefix(stree);

                } else {
                    // artificial large node
                    if(stree->chain_remain == MAXDISTANCE) {

                        SET_SUFFIXLINK(stree->inner.next_num + LARGE_WIDTH);
                        completelarge(stree);

                    } else {
                        if(stree->chainstart == NULL) {
                            // Start new chain
                            stree->chainstart = stree->inner.next;
                        }
                        stree->chain_remain++;
                        stree->inner.next += SMALL_WIDTH;      // case (2.2.4)
                        stree->inner.next_num += SMALL_WIDTH;
                        stree->n_small++;
                    }
                }
            }
        }

        insert_vertex(stree);

    }

    stree->chainstart = NULL;
    linkrootchildren(stree);

    return 0;
}

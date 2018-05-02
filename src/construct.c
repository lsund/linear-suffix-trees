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


///////////////////////////////////////////////////////////////////////////////
// Private


static bool last_suffix(STree *stree)
{
    return stree->tailptr >= stree->sentinel;
}


static bool head_is_node(STree *stree)
{
    return stree->headend == NULL;
}


static bool root_depth(STree *stree)
{
    return stree->headnodedepth == 0;
}


static bool head_is_root(STree *stree)
{
    return root_depth(stree) && head_is_node(stree);
}


static bool empty_head(STree *stree)
{
    return stree->headstart == stree->headend;
}


static void insert_vertex(STree *stree)
{
    if(head_is_node(stree)) {
        insertleaf(stree);
    } else {
        insertbranchnode(stree);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Public


Sint construct(STree *stree,wchar_t *text,Uint textlen)
{
    stree->nonmaximal = NULL;

    init(stree,text,textlen);

    while(!last_suffix(stree) || !head_is_root(stree)) {

        // case (1)
        if(head_is_root(stree)) {

            (stree->tailptr)++;
            scanprefix(stree);

        // Case 2
        } else {
            // Case 2.1: Head is node
            if(head_is_node(stree)) {

                FOLLOWSUFFIXLINK;
                scanprefix(stree);

            // Case 2.2
            } else {
                // Case 2.2.1: at root, do not use links
                if(root_depth(stree))
                {
                    // No need to rescan
                    if(empty_head(stree)) {
                        stree->headend = NULL;
                    } else {

                        (stree->headstart)++;
                        rescan(stree);

                    }
                // Case 2.2.2
                } else {
                    FOLLOWSUFFIXLINK;
                    rescan(stree);
                }
                // Case 2.2.3
                if(head_is_node(stree)) {

                    SETSUFFIXLINK(INDEX_INNER(stree,stree->headnode));
                    completelarge(stree);
                    scanprefix(stree);

                } else {
                    // artificial large node
                    if(stree->smallnotcompleted == MAXDISTANCE) {

                        SETSUFFIXLINK(stree->inner_vertices.next_free_num + LARGEINTS);
                        completelarge(stree);

                    } else {
                        if(stree->chainstart == NULL) {
                            // Start new chain
                            stree->chainstart = stree->inner_vertices.next_free;
                        }
                        (stree->smallnotcompleted)++;
                        (stree->inner_vertices.next_free) += SMALLINTS;      // case (2.2.4)
                        (stree->inner_vertices.next_free_num) += SMALLINTS;
                        stree->smallnode++;
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

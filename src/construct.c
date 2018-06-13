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


static void insert_leafedge(STree *stree)
{
    if(base_is_vertex(stree)) {
        insert(stree);
    } else {
        split_and_insert(stree);
    }
}


static void append_chain(STree *stree)
{
    if (!stree->chain.first) {
        init_chain(stree);
    }
    stree->chain.size     += 1;
    stree->inner.next     += SMALL_VERTEXSIZE;
    stree->inner.next_ind += SMALL_VERTEXSIZE;
}

static Bool label_empty(Label label)
{
    return label.start == label.end;
}

static void find_base(STree *stree)
{
    if (head_label_depth(stree) == 0) {
        if (label_empty(stree->head.label)) {
            stree->head.label.end = NULL;
        } else {
            stree->head.label.start++;
            skip_count(stree);
        }

    } else {
        follow_link(stree);
        skip_count(stree);
    }
}

static void find_next_head(STree * stree) {
    if(head_label_depth(stree) == 0 && base_is_vertex(stree)) {
        stree->tail++;
        scan_tail(stree);
    } else {
        if (is_head_old(stree)) {
            follow_link(stree);
            scan_tail(stree);
        } else {
            find_base(stree);
            if(base_is_vertex(stree)) {
                set_chain_distances(stree);
                scan_tail(stree);
            } else {
                append_chain(stree);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


void construct(STree *stree)
{
    init(stree);
    while(!IS_SENTINEL(stree->tail)) {
        find_next_head(stree);
        insert_leafedge(stree);
    }
}

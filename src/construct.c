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


static void insert_leafedge(STree *st)
{
    if(base_is_vertex(st)) {
        insert(st);
    } else {
        split_and_insert(st);
    }
}


static void append_chain(STree *st)
{
    if (!st->chain.fst) {
        init_chain(st);
    }
    st->chain.size     += 1;
    st->is.nxt     += SMALL_VERTEXSIZE;
    st->is.nxt_ind += SMALL_VERTEXSIZE;
}

static Bool label_empty(Label label)
{
    return label.start == label.end;
}

static void find_base(STree *st)
{
    if (head_is_root(st)) {
        if (label_empty(st->head.label)) {
            st->head.label.end = NULL;
        } else {
            st->head.label.start++;
            skip_count(st);
        }

    } else {
        follow_link(st);
        skip_count(st);
    }
}

static void find_nxt_head(STree * st) {
    if(head_is_root(st) && base_is_vertex(st)) {
        st->tail++;
        scan_tail(st);
    } else {
        if (is_head_old(st)) {
            follow_link(st);
            scan_tail(st);
        } else {
            find_base(st);
            if(base_is_vertex(st)) {
                set_chain_distances(st);
                scan_tail(st);
            } else {
                append_chain(st);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


void construct(STree *st)
{
    init(st);
    while(!IS_SENTINEL(st->tail)) {
        find_nxt_head(st);
        insert_leafedge(st);
    }
}

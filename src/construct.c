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
#include "insert.h"
#include "skip_count.h"
#include "scan.h"

Uint small_count = 0;
Uint large_count = 0;
Uint leaf_count = 0;

///////////////////////////////////////////////////////////////////////////////
// Private


static void insert_tailedge(STree *st)
{
    leaf_count++;
    if(base_is_vertex(st)) {
        insert(st);
    } else {
        split_and_insert(st);
    }
}


static bool label_empty(Label label)
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
                large_count++;
            } else {
                append_chain(st);
                small_count++;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


void construct(STree *st)
{
    large_count = 0;
    small_count = 0;
    leaf_count = 0;
    init(st);
    while(!tail_at_lastchar(st)) {
        find_nxt_head(st);
        insert_tailedge(st);
    }
    /* printf("%lu\n", small_count + large_count); */
}

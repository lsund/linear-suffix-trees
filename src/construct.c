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
    if(is_head_vertex(stree)) {
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
    stree->inner.next_ind += SMALL_VERTEXSIZE;
}

static Bool label_empty(Label label)
{
    return label.start == label.end;
}


///////////////////////////////////////////////////////////////////////////////
// Public


Sint construct(STree *stree)
{
    // `init` gives the suffix tree structure its default values.
    init(stree);

    while(!IS_SENTINEL(stree->tail)) {

        // head(i - 1) at the root vertex
        if(head_depth(stree) == 0 && is_head_vertex(stree)) {
            // Scan the maximal prefix of the current suffix = tail from the
            // root. We thus compute the head/tail by scanning.
            stree->tail++;
            scan_tail(stree);

        // If head is not the root then head(i-1) = xw. We know that w is a
        // prefix of head(i) and the location of w, so we can find w using
        // suffix links.
        //
        // If The head is at a vertex, just follow the suffix link and scan the
        // tail from there.
        } else if (is_head_vertex(stree)) {

            follow_link(stree);
            scan_tail(stree);

        } else {

            // We know that head exists under the suffix link, so we can use
            // skip-count to find it. First check if we are at the root.
            if(head_depth(stree) == 0) {
                // At root
                if (label_empty(stree->head.label)) {
                    // Skip-count is not necessary
                    stree->head.label.end = NULL;
                } else {
                    stree->head.label.start++;
                    skip_count(stree);
                }
            } else {
                // Check so its not a rootedge before following the suffix link
                follow_link(stree);
                skip_count(stree);
            }

            // We have now computed w.
            // We can decide weather the head(i - 1) is small or large. If w
            // is at a node, then its head position is smaller than i - 1.
            // Therefore, head(i - 1) is large so the current chain ending with
            // head(i - 1) can be collapsed and a new one started.
            if(is_head_vertex(stree)) {
                finalize_chain(stree);
                scan_tail(stree);
            } else {
                // ... Otherwise, the head location is already found.
                // simply grow the chain ...
                grow_chain(stree);
            }
        }
        // ... and insert the new vertex
        insert_vertex(stree);
    }

    stree->chain.first = NULL;
    linkrootchildren(stree);

    return 0;
}

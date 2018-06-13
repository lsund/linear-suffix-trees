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
    if(head_ends_on_vertex(stree)) {
        insert(stree);
    } else {
        split_and_insert(stree);
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

static void find_base(STree *stree)
{
    // Suppose u = parent(head(i)) and let
    //         av = label(u, head(i)).
    // Then    w = link(u)av
    if (head_label_depth(stree) == 0) {

        // Consider the case that head(i) was created on a root edge by
        // the previous phase.
        if (label_empty(stree->head.label)) {
            // For an empty label av, no need to search anything.
            stree->head.label.end = NULL;
        } else {
            // Otherwise, v is a prefix of head(i + 1). skip count
            // v down from the root.
            stree->head.label.start++;
            skip_count(stree);
        }

    } else {

        // Otherwise, the suffix link for u can be followed, and
        // then skip-count av down from link(u).
        follow_link(stree);
        skip_count(stree);
    }
}

static void find_next_head(STree * stree) {
    // Consider the case when head(i) is the root.
    if(head_label_depth(stree) == 0 && head_ends_on_vertex(stree)) {

        // Since head(i) is at the root, tail(i) spells out the entire suffix. Thus
        // to find head(i + 1), increment tail to get the next suffix and scan a
        // maximal prefix from the rootin.
        stree->tail++;
        scan_tail(stree);

    // Otherwise, it exploits the fact that w is a prefix of head(i + 1),
    // if head(i) = aw, and that w can be found using suffix links.
    } else {

        // Distinguish between the two cases when (1) head(i) was created
        // by the last phase, and head(i) was created by some other phase.
        if (is_head_old(stree)) {

            // Case (1) If head(i) was constructed prior to thein the last
            // phase, then the suffix link to w already exists in the tree
            // which can just be followed and then scan the tail as before.
            follow_link(stree);
            scan_tail(stree);

        } else {

            // Case (2), head(i) was in fact created by the previous phase.
            // The problem is that the suffix link from head(i) does not
            // yet exist and w now has to be found in an other way. Note that
            // head(i) is the only vertex that does not have a suffix link,
            // so the parents link can be used instead.
            //
            find_base(stree);
            // w is now computed, and have enough information to decide
            // wether head(i) is small or large. If w is at a node, then
            // its head position is smaller than i. Therefore,
            // head(i) is large so the current chain ending with
            // head(i) can be finilized and a new one started.
            if(head_ends_on_vertex(stree)) {
                finalize_chain(stree);
                // To find head(i + 1), it suffices to scan the tail down
                // from w.
                scan_tail(stree);
            } else {
                // Otherwise, head(i + 1) is already found. head(i) is
                // small and implicitly added to the chain.
                grow_chain(stree);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


void construct(STree *stree)
{
    // `init` allocates initial memory and gives the suffix tree fields default
    // values.
    init(stree);

    // The last suffix is $. Since the character is unique, the root leaf
    // that starts with it is not inserted until the last iteration, so
    // stree->tail will never start with `$` until the last iteration.
    while(!IS_SENTINEL(stree->tail)) {

        // In the i'th iteration, the task of the algorithm is to locate
        // head(i + 1) and then insert a new leaf edge at that point. To do
        // that, it first looks at head(i).
        find_next_head(stree);

        // Now finally insert the new leaf vertex.
        insert_leafedge(stree);
    }
}

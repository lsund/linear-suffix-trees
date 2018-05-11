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
    // `init` allocates initial memory and gives the suffix tree fields default
    // values.
    init(stree);

    // The last suffix is just `$`. Since it is unique, it does not exist as a
    // root leaf. Therefore, `stree->tail` will consist of the sentinel
    // character only at the last iteration.
    while(!IS_SENTINEL(stree->tail)) {

        // In the i'th iteration, the task of the algorithm is to locate
        // head(i + 1) and inserts a new edge at this point.

        // head(i) is the root.
        if(head_depth(stree) == 0 && is_head_vertex(stree)) {

            // Since head is at the root, tail has to increment by one to
            // consider the next suffix.
            stree->tail++;
            // Then to find head(i + 1), scan the maximal prefix of tail(i)
            // starting at the root.
            scan_tail(stree);

        // When head(i) is not empty, then head(i) = `t[i]w` for the character
        // t[i] and the string w then one exploits the fact that w is a prefix
        // of head(i + 1), and that the location of w can be found using suffix
        // links.
        } else {

            // If head(i) is not the last vertex that was created, then there
            // already exists a suffix link from head(i) to `w` ...
            if (is_head_vertex(stree)) {

                // ... which can just be followed ...
                follow_link(stree);
                // and then scanning the tail as before.
                scan_tail(stree);

            // If head(i) was the last vertex created, then the suffix link
            // does not yet exist. Let us call the edge label leading up to
            // head(i) for `av`.
            } else {

                // First, `w` has to be found.

                // If head(i) is on a root edge, just skip-count down from the
                // root, since `v` is a prefix of head(i + 1).
                if(head_depth(stree) == 0) {

                    if (label_empty(stree->head.label)) {
                        // For an empty label, no need to search anything.
                        stree->head.label.end = NULL;
                    } else {
                        // Don't want to scan the first character of head(i).
                        stree->head.label.start++;
                        skip_count(stree);
                    }

                } else {

                    // Let u = parent(head(i)). The suffix link for u exists,
                    // and this can be followed.
                    follow_link(stree);
                    // Then, skip count the string av from that point.
                    skip_count(stree);
                }

                // w is now computed, and have enough information to decide
                // wether head(i) is small or large. If w is at a node, then
                // its head position is smaller than i. Therefore,
                // head(i) is large so the current chain ending with
                // head(i) can be finilized and a new one started.
                if(is_head_vertex(stree)) {
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
        // Now finally insert the new leaf vertex.
        insert_vertex(stree);
    }

    /* stree->chain.first = NULL; */
    /* linkrootchildren(stree); */

    return 0;
}

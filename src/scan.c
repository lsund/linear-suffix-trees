#include "scan.h"


static Uint lcp_with_tail(STree *st, Wchar *start, Wchar *end)
{
    Pattern tailpatt = make_patt(st->tl + 1, text.lst - 1);
    return 1 + lcp(tailpatt, make_patt(start, end));
}


static Wchar get_label(STree *st, Uint offset, Wchar **label)
{
    *label = text.fst + (st->hd.d + offset);
    return **label;
}


///////////////////////////////////////////////////////////////////////////////
// Public


// Scans a prefix of the current tail from the current base vertex
void scan_tail(STree *st)
{
    Wchar fst              = 0;
    Uint plen, depth, dist = 0;
    Vertex child           = 0;
    Wchar *label           = NULL;
    VertexP chain_term     = NULL;
    VertexP curr_vertex    = NULL;

    if(head_is_root(st)) {

        if(tail_at_lastchar(st)) {
            st->hd.l.end = NULL;
            return;
        }

        // Get the
        fst = *(st->tl);
        child = ROOT_CHILD(fst);
        if(child == UNDEF) {
            st->hd.l.end = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(child)) {

            Wchar *suffix_start = text.fst + VERTEX_TO_INDEX(child) + 1;

            Pattern patt = make_patt(suffix_start, text.lst - 1);
            Pattern tailpatt = make_patt(st->tl + 1, text.lst - 1);
            plen = lcp(patt, tailpatt) + 1;

            st->tl += plen;

            update_head_and_splitloc(
                    st,
                    patt.start - 1,
                    patt.start - 1 + plen - 1,
                    child,
                    st->split.left
                );

            return;
        } else {

            // The root edge is an inner vertex
            curr_vertex = VERTEX_TO_REF(child);

            set_dist_and_chainterm(st, curr_vertex, &chain_term, &dist);
            Uint hp = get_headpos(st, curr_vertex, dist, chain_term);
            depth   = get_depth(st, curr_vertex, dist, chain_term);
            label   = text.fst + hp;
            plen    = lcp_with_tail(st, label + 1, label + depth - 1);

            st->tl += plen;
            if (depth > plen) {
                // cannot reach the child, fall out of tree
                Vertex left = st->split.left;
                update_head_and_splitloc(st, label, label + plen - 1, child, left);
                return;
            }
            st->hd.v = curr_vertex;
            st->hd.d = depth;
        }
    }

    // Head is not the root
    while(true) {

        Vertex prev = UNDEF;
        child = CHILD(st->hd.v);
        fst = *st->tl;
        Wchar edge_c;

        do {
            // Iterate over all children, find the correct edge
            if(IS_LEAF(child)) {

                Uint leaf_ind = VERTEX_TO_INDEX(child);
                edge_c = get_label(st, leaf_ind, &label);

                if (edge_c >= fst) {
                    break;
                }

                prev  = child;
                child = LEAF_SIBLING(st->ls.fst + leaf_ind);

            } else {

                curr_vertex = VERTEX_TO_REF(child);
                set_dist_and_chainterm(st, curr_vertex, &chain_term, &dist);

                Uint hp  = get_headpos(st, curr_vertex, dist, chain_term);
                edge_c = get_label(st, hp, &label);

                if (edge_c >= fst) {
                    break;
                }

                prev  = child;
                child = SIBLING(curr_vertex);
            }

        } while(EXISTS(child));

        if (!EXISTS(child) || edge_c > fst) {
            // No matching a-edge found
            // New edge will become right sibling of last vertex
            st->split.left = prev;
            st->hd.l.end   = NULL;
            return;
        } else {
            // `child` is the child of the matching a-edge
            if (IS_LEAF(child)) {
                plen = lcp_with_tail(st, label + 1, text.lst - 1);
                st->tl += plen;
                update_head_and_splitloc(st, label, label + plen - 1, child, prev);
                return;
            }

            depth   = get_depth(st, curr_vertex, dist, chain_term);
            Uint edge_len = depth - st->hd.d;
            plen    = lcp_with_tail(st, label + 1, label + edge_len - 1);
            st->tl += plen;

            // Nothing more to scan
            if(edge_len > plen) {
                update_head_and_splitloc(st, label, label + plen - 1, child, prev);
                return;
            }

            st->hd.v = curr_vertex;
            st->hd.d = depth;
        }
        // Reiterate...
    }
}

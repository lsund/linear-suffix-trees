#include "skip_count.h"

void skip_count(STree *st)
{
    Wchar fst;
    Uint depth;
    VertexP curr_vertex          = NULL;
    VertexP chain_term = NULL;
    Uint dist = 0;

    if (head_is_root(st)) {

        fst = *(st->hd.l.start);
        Uint root_child = st->rs[(Uint) fst];

        if (IS_LEAF(root_child)) {

            // Head is on a root edge which leads to a leaf
            st->split.child = root_child;
            return;

        } else {

            // Head is on root a edge but not a leaf
            curr_vertex = VERTEX_TO_REF(root_child);
            set_dist_and_chainterm(st, curr_vertex, &chain_term, &dist);

            depth           = get_depth(st, curr_vertex, dist, chain_term);
            Uint base_depth = st->hd.l.end - st->hd.l.start + 1;

            if(depth > base_depth) {
                // cannot reach the child
                st->split.child = root_child;
                return;
            } else {
                // Update head to the child
                st->hd.v = curr_vertex;
                st->hd.d = depth;
            }

            if(depth == base_depth) {
                // Location found
                st->hd.l.end = NULL;
                return;
            } else {
                st->hd.l.start += depth;
            }
        }
    }

    // The new head was not found directly under root
    while(true) {

        fst              = *st->hd.l.start;
        Vertex prev      = UNDEF;
        Vertex v_child = CHILD(st->hd.v);

        // iterate over the children
        while(true) {

            Wchar edge_c;
            Uint leaf_ind;
            if(IS_LEAF(v_child)) {

                // Child is a leaf
                leaf_ind = VERTEX_TO_INDEX(v_child);
                edge_c = text.fst[st->hd.d + leaf_ind];

                if(edge_c == fst) {
                    // correct edge found
                    st->split.child = v_child;
                    st->split.left = prev;
                    return;
                }

                prev = v_child;
                v_child = st->ls.fst[leaf_ind];
                continue;

            } else {

                // Child is an inner vertex
                curr_vertex = VERTEX_TO_REF(v_child);
                set_dist_and_chainterm(st, curr_vertex, &chain_term, &dist);
                Vertex hd = get_headpos(st, curr_vertex, dist, chain_term);
                edge_c = text.fst[st->hd.d + hd];
                // Correct edge found
                if(edge_c == fst) {
                    break;
                }
                prev = v_child;
                v_child = SIBLING(curr_vertex);
            }
        }

        // At this point, the correct edge has been found.
        depth           = get_depth(st, curr_vertex, dist, chain_term);
        Uint edge_len   = depth - st->hd.d;
        Uint base_depth = st->hd.l.end - st->hd.l.start + 1;
        if(edge_len > base_depth) {
            // cannot reach the child
            st->split.child = v_child;
            st->split.left = prev;
            return;
        }
        // The head becomes the next vertex
        st->hd.v = curr_vertex;
        st->hd.d = depth;
        if(edge_len == base_depth) {
            // Location found
            st->hd.l.end = NULL;
            return;
        }
        st->hd.l.start += edge_len;
        // Iterate...
    }
}

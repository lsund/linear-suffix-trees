#include "scan.h"

Uint prefixlen(Wchar *start, Pattern *patt, Uint remain)
{
    if (remain < 0) {
        remain = 1;
    }
    Pattern textpatt  = make_patt(start + remain, text.lst - 1);
    Pattern curr_patt = make_patt(patt->start + remain, patt-> end);
    return remain + lcp(textpatt, curr_patt);
}


static Uint lcp_with_tail(STree *st, Wchar *start, Wchar *end)
{
    Pattern tailpatt = make_patt(st->tl + 1, text.lst - 1);
    return 1 + lcp(tailpatt, make_patt(start, end));
}


static void update_stree(STree *st, Wchar *label_start, Uint plen, Uint v, Uint prev)
{
    st->hd.l.start = label_start;
    st->hd.l.end = label_start + (plen-1);
    st->split.child = v;
    st->split.left = prev;
}


static Wchar get_label(STree *st, Uint offset, Wchar **label_start)
{
    *label_start = text.fst + (st->hd.d + offset);
    return **label_start;
}


///////////////////////////////////////////////////////////////////////////////
// Public


// Scans a prefix of the current tail down from a given node
void scan_tail(STree *st)
{
    VertexP chain_term = NULL;
    Uint leafindex;
    Uint depth;
    Uint edgelen;
    Vertex current_vertex;
    VertexP current_vertexp = NULL;
    Uint dist = 0;
    Uint prev;
    Uint plen;
    Uint hd;
    Wchar *label_start = NULL;
    Wchar fst;
    Wchar labelchar = 0;

    if(head_is_root(st)) {

        // There is no $-edge
        if(tail_at_lastchar(st)) {
            st->hd.l.end = NULL;
            return;
        }

        fst = *(st->tl);
        current_vertex = ROOT_CHILD(fst);
        if(current_vertex == UNDEF) {
            st->hd.l.end = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(current_vertex)) {

            Wchar *suffix_start = text.fst + VERTEX_TO_INDEX(current_vertex) + 1;

            Pattern edgepatt = make_patt(suffix_start, text.lst - 1);
            Pattern tailpatt = make_patt(st->tl + 1, text.lst - 1);
            plen = lcp(edgepatt, tailpatt) + 1;

            st->tl += plen;
            st->hd.l.start   = edgepatt.start - 1;
            st->hd.l.end     = edgepatt.start - 1 + (plen-1);
            st->split.child = current_vertex;

            return;
        }

        current_vertexp = VERTEX_TO_REF(current_vertex);

        set_dist_and_chainterm(st, current_vertexp, &chain_term, &dist);
        hd = get_headpos(st, current_vertexp, dist, chain_term);
        depth = get_depth(st, current_vertexp, dist, chain_term);

        label_start = text.fst + hd;
        plen = lcp_with_tail(st, label_start + 1, label_start + depth - 1);

        st->tl+= plen;
        if(depth > plen) {

            // cannot reach the successor, fall out of tree
            st->split.child     = current_vertex;
            st->hd.l.start = label_start;
            st->hd.l.end   = label_start + (plen - 1);
            return;
        }
        st->hd.v = current_vertexp;
        st->hd.d = depth;
    }

    // Head is not the root
    while(true) {

        prev = UNDEF;
        current_vertex = CHILD(st->hd.v);
        fst = *(st->tl);

        do {
            // find successor edge with fst = fst
            if(IS_LEAF(current_vertex)) {

                leafindex = VERTEX_TO_INDEX(current_vertex);
                labelchar = get_label(st, leafindex, &label_start);

                if (labelchar >= fst) {
                    break;
                }

                prev          = current_vertex;
                current_vertex = LEAF_SIBLING(st->ls.fst + leafindex);

            } else {

                current_vertexp   = VERTEX_TO_REF(current_vertex);
                set_dist_and_chainterm(st, current_vertexp, &chain_term, &dist);

                hd      = get_headpos(st, current_vertexp, dist, chain_term);
                labelchar = get_label(st, hd, &label_start);

                if (labelchar >= fst) {
                    break;
                }

                prev          = current_vertex;
                current_vertex = SIBLING(current_vertexp);
            }

        } while(EXISTS(current_vertex));

        if (!EXISTS(current_vertex) || labelchar > fst) {

            // No matching a-edge found
            // New edge will become right sibling of last vertex
            st->split.left = prev;
            st->hd.l.end = NULL;
            return;
        }

        if (IS_LEAF(current_vertex)) {
            plen = lcp_with_tail(st, label_start + 1, text.lst - 1);
            (st->tl) += plen;
            update_stree(st, label_start, plen, current_vertex, prev);
            return;
        }

        depth   = get_depth(st, current_vertexp, dist, chain_term);
        edgelen = depth - st->hd.d;
        plen    = lcp_with_tail(st, label_start + 1, label_start + edgelen - 1);
        (st->tl) += plen;

        // cannot reach nxt node
        if(edgelen > plen) {
            update_stree(st, label_start, plen, current_vertex, prev);
            return;
        }

        st->hd.v = current_vertexp;
        st->hd.d = depth;
    }
}

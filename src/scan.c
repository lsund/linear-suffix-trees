#include "scan.h"

Uint iterate = 0;

///////////////////////////////////////////////////////////////////////////////
// Private


static void skip_edge(
        Loc *loc, Uint *v, Pattern *patt, Uint *depth, Uint hd, Uint plen, Uint edgelen)
{
    loc->string.start  = hd;
    loc->string.length = *depth + plen;
    patt->start        += edgelen;
    *depth             += edgelen;
    loc->prev          = loc->nxt;
    loc->nxt          = v;
    loc->remain        = 0;
}

static Uint prefixlen(Wchar *start, Pattern *patt, Uint remain)
{
    if (remain < 0) {
        remain = 1;
    }
    Pattern textpatt  = make_patt(start + remain, text.lst - 1);
    Pattern curr_patt = make_patt(patt->start + remain, patt-> end);
    Uint lcp_res      = lcp(textpatt, curr_patt);
    return remain + lcp_res;
}


static Uint  match_leaf(Loc *loc, Uint v, Pattern *patt, Uint remain)
{
    Uint leafnum = VERTEX_TO_INDEX(v);
    loc->fst   = text.fst + leafnum;

    return prefixlen(loc->fst, patt, remain);
}


static void find_last_successor(STree *st, Vertex *prev_p, Vertex v)
{
    Vertex prev = *prev_p;
    do {
        prev = v;
        if(IS_LEAF(v)) {
            v = LEAF_SIBLING(VERTEX_TO_LEAFREF(v));
        } else {
            v = SIBLING(VERTEX_TO_REF(v));
        }
    } while(IS_SOMETHING(v));
    *prev_p = prev;
}


static void update_st(STree *st, Wchar *label_start, Uint plen, Uint v, Uint prev)
{
    st->hd.l.start = label_start;
    st->hd.l.end = label_start + (plen-1);
    st->split.child = v;
    st->split.left = prev;
}


static Uint tail_prefixlen(STree *st, Wchar *start, Wchar *end)
{
    Pattern tailpatt = make_patt(st->tail + 1, text.lst - 1);
    return 1 + lcp(tailpatt, make_patt(start, end));
}


static Wchar get_label(STree *st, Uint offset, Wchar **label_start)
{
    *label_start = text.fst + (st->hd.d + offset);
    return **label_start;
}


///////////////////////////////////////////////////////////////////////////////
// Public


Wchar *scan(STree *st, Loc *loc, Uint *start_vertex, Pattern patt)
{
    VertexP vertexp  = start_vertex;
    VertexP chainend = NULL;
    Vertex  hd     = 0;
    Uint depth       = 0;
    Uint distance    = 0;
    Uint remain      = 0;
    Wchar fstchar  = 0;
    Uint edgelen     = 0;

    if(!IS_ROOT(vertexp)) {

        set_dist_and_chainend(st, vertexp, &chainend, &distance);
        hd = get_headpos(st, vertexp, &chainend, distance);

        set_dist_and_chainend(st, vertexp, &chainend, &distance);
        depth = get_depth(st, vertexp, distance, &chainend);
    }

    init_loc(vertexp, hd, depth, loc);

    while(true) {

        if (patt.start > patt.end) {
            return NULL;
        }

        fstchar    = *patt.start;
        Uint leafnum = 0;
        Wchar *label = NULL;
        Uint plen    = 0;

        if(IS_ROOT(vertexp)) {

            Vertex rootchild = ROOT_CHILD(fstchar);

            if (!EXISTS(rootchild)) {
                return patt.start;
            }

            if(IS_LEAF(rootchild)) {

                plen = match_leaf(loc, rootchild, &patt, remain);

                loc->leafedge = true;
                if(MATCHED(plen, patt.end, patt.start)) {
                    return NULL;
                } else {
                    return patt.start + plen;
                }
            }

            vertexp = VERTEX_TO_REF(rootchild);

            set_dist_and_chainend(st, vertexp, &chainend, &distance);
            hd = get_headpos(st, vertexp, &chainend, distance);

            label   = LABEL_START(hd);

        } else {

            Wchar labelchar;
            Uint v = CHILD(vertexp);

            while(true) {

                if (IS_NOTHING(v)) {

                    return patt.start;

                } else if (IS_LEAF(v)) {

                    leafnum = VERTEX_TO_INDEX(v);
                    label   = LABEL_START(depth + leafnum);

                    if(IS_LAST(label)) {
                        return patt.start;
                    }

                    labelchar = *label;
                    if(labelchar > fstchar) {
                        return patt.start;
                    }

                    if(labelchar == fstchar) {

                        loc->leafedge = true;
                        plen = prefixlen(label, &patt, remain);
                        if(MATCHED(plen, patt.end, patt.start)) {
                            return NULL;
                        } else {
                            loc->nxt = VERTEX_TO_LEAFREF(v);
                            return patt.start + plen;
                        }
                    }

                    v = LEAF_SIBLING(VERTEX_TO_LEAFREF(leafnum));

                } else {

                    vertexp  = VERTEX_TO_REF(v);

                    set_dist_and_chainend(st, vertexp, &chainend, &distance);
                    hd = get_headpos(st, vertexp, &chainend, distance);

                    label    = LABEL_START(depth + hd);
                    labelchar = *label;

                    if (labelchar > fstchar) {
                        return patt.start;
                    }

                    if(labelchar == fstchar) {
                        break;
                    }

                    v = SIBLING(vertexp);
                }
            }
        }

        Uint prevdepth = depth;

        set_dist_and_chainend(st, vertexp, &chainend, &distance);
        depth = get_depth(st, vertexp, distance, &chainend);
        edgelen = depth - prevdepth;
        loc->edgelen = edgelen;

        if(remain > 0) {
            if(remain >= edgelen) {
                plen = edgelen;
                remain -= plen;
            } else {
                Pattern rem_patt  = patt_inc(patt, remain);
                Pattern labelpatt = make_patt(label + remain, label + edgelen - 1);
                Uint lcp_res      = lcp(rem_patt, labelpatt);
                plen              = remain + lcp_res;
                remain            = 0;
            }
        } else {
            Pattern labelpatt = make_patt(label + 1, label + edgelen - 1);
            plen = 1 + lcp(patt_inc(patt, 1), labelpatt);
        }


        if(plen == edgelen) {

            skip_edge(loc, vertexp, &patt, &prevdepth, hd, plen, edgelen);

        } else {

            update_loc(vertexp, hd, plen, label, prevdepth, edgelen, loc);

            if(MATCHED(plen, patt.end, patt.start)) {
                return NULL;
            }

            return patt.start + plen;
        }
    }
}


// Scans a prefix of the current tail down from a given node
void scan_tail(STree *st)
{
    VertexP chainend = NULL;
    Uint leafindex;
    Uint depth;
    Uint edgelen;
    Vertex current_vertex;
    VertexP current_vertexp = NULL;
    Uint distance = 0;
    Uint prev;
    Uint plen;
    Uint hd;
    Wchar *label_start = NULL;
    Wchar fstchar;
    Wchar labelchar = 0;

    if(head_is_root(st)) {

        // There is no $-edge
        if(tail_at_lastchar(st)) {
            st->hd.l.end = NULL;
            return;
        }

        fstchar = *(st->tail);
        current_vertex = ROOT_CHILD(fstchar);
        if(current_vertex == UNDEF) {
            st->hd.l.end = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(current_vertex)) {

            Wchar *suffix_start = text.fst + VERTEX_TO_INDEX(current_vertex) + 1;

            Pattern edgepatt = make_patt(suffix_start, text.lst - 1);
            Pattern tailpatt = make_patt(st->tail + 1, text.lst - 1);
            plen = lcp(edgepatt, tailpatt) + 1;

            st->tail += plen;
            st->hd.l.start   = edgepatt.start - 1;
            st->hd.l.end     = edgepatt.start - 1 + (plen-1);
            st->split.child = current_vertex;

            return;
        }

        current_vertexp = VERTEX_TO_REF(current_vertex);

        set_dist_and_chainend(st, current_vertexp, &chainend, &distance);
        hd = get_headpos(st, current_vertexp, &chainend, distance);
        depth = get_depth(st, current_vertexp, distance, &chainend);

        label_start = text.fst + hd;
        plen = tail_prefixlen(st, label_start + 1, label_start + depth - 1);

        st->tail+= plen;
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
        fstchar = *(st->tail);

        do {
            iterate++;
            // find successor edge with fstchar = fstchar
            if(IS_LEAF(current_vertex)) {

                leafindex = VERTEX_TO_INDEX(current_vertex);
                labelchar = get_label(st, leafindex, &label_start);

                if (labelchar >= fstchar) {
                    break;
                }

                prev          = current_vertex;
                current_vertex = LEAF_SIBLING(st->ls.fst + leafindex);

            } else {

                current_vertexp   = VERTEX_TO_REF(current_vertex);
                set_dist_and_chainend(st, current_vertexp, &chainend, &distance);

                hd      = get_headpos(st, current_vertexp, &chainend, distance);
                labelchar = get_label(st, hd, &label_start);

                if (labelchar >= fstchar) {
                    break;
                }

                prev          = current_vertex;
                current_vertex = SIBLING(current_vertexp);
            }

        } while(IS_SOMETHING(current_vertex));

        if (IS_NOTHING(current_vertex) || labelchar > fstchar) {

            // No matching a-edge found
            // New edge will become right sibling of last vertex
            st->split.left = prev;
            st->hd.l.end = NULL;
            return;
        }

        if (IS_LEAF(current_vertex)) {
            plen = tail_prefixlen(st, label_start + 1, text.lst - 1);
            (st->tail) += plen;
            update_st(st, label_start, plen, current_vertex, prev);
            return;
        }

        depth   = get_depth(st, current_vertexp, distance, &chainend);
        edgelen = depth - st->hd.d;
        plen    = tail_prefixlen(st, label_start + 1, label_start + edgelen - 1);
        (st->tail) += plen;

        // cannot reach nxt node
        if(edgelen > plen) {
            update_st(st, label_start, plen, current_vertex, prev);
            return;
        }

        st->hd.v = current_vertexp;
        st->hd.d = depth;
    }
}

#include "test.h"

static Uint match_leaf(Loc *loc, Uint v, Pattern *patt, Uint remain)
{
    Uint leafnum = VERTEX_TO_INDEX(v);
    loc->fst   = text.fst + leafnum;

    return prefixlen(loc->fst, patt, remain);
}


static void skip_edge(Loc *loc, Uint *v, Pattern *patt, Uint *depth, Uint hd,
        Uint plen, Uint edgelen)
{
    loc->s       = hd;
    loc->d       = *depth + plen;
    patt->start += edgelen;
    *depth      += edgelen;
    loc->prev    = loc->nxt;
    loc->nxt     = v;
    loc->rem     = 0;
}

Wchar *scan(STree *st, Loc *loc, Uint *start_vertex, Pattern patt)
{
    VertexP vertexp    = start_vertex;
    VertexP chain_term = NULL;
    Vertex  hd         = 0;
    Uint depth         = 0;
    Uint dist          = 0;
    Uint remain        = 0;
    Wchar fst          = 0;
    Uint edgelen       = 0;

    if(!IS_ROOT(vertexp)) {

        set_dist_and_chainterm(st, vertexp, &chain_term, &dist);
        hd = get_headpos(st, vertexp, dist, chain_term);
        depth = get_depth(st, vertexp, dist, chain_term);
    }

    init_location(vertexp, hd, depth, loc);

    while(true) {

        if (patt.start > patt.end) {
            return NULL;
        }

        fst          = *patt.start;
        Uint leafnum = 0;
        Wchar *label = NULL;
        Uint plen    = 0;

        if(IS_ROOT(vertexp)) {

            Vertex rootchild = ROOT_CHILD(fst);

            if (!EXISTS(rootchild)) {
                return patt.start;
            }

            if(IS_LEAF(rootchild)) {

                plen = match_leaf(loc, rootchild, &patt, remain);

                loc->isleaf = true;
                if(MATCHED(plen, patt.end, patt.start)) {
                    return NULL;
                } else {
                    return patt.start + plen;
                }
            }

            vertexp = VERTEX_TO_REF(rootchild);

            set_dist_and_chainterm(st, vertexp, &chain_term, &dist);
            hd = get_headpos(st, vertexp, dist, chain_term);

            label   = LABEL(hd);

        } else {

            Wchar labelchar;
            Uint v = CHILD(vertexp);

            while(true) {

                if (!EXISTS(v)) {

                    return patt.start;

                } else if (IS_LEAF(v)) {

                    leafnum = VERTEX_TO_INDEX(v);
                    label   = LABEL(depth + leafnum);

                    if(label >= text.lst) {
                        return patt.start;
                    }

                    labelchar = *label;
                    if(labelchar > fst) {
                        return patt.start;
                    }

                    if(labelchar == fst) {

                        loc->isleaf = true;
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

                    set_dist_and_chainterm(st, vertexp, &chain_term, &dist);
                    hd = get_headpos(st, vertexp, dist, chain_term);

                    label    = LABEL(depth + hd);
                    labelchar = *label;

                    if (labelchar > fst) {
                        return patt.start;
                    }

                    if(labelchar == fst) {
                        break;
                    }

                    v = SIBLING(vertexp);
                }
            }
        }

        Uint prevdepth = depth;

        set_dist_and_chainterm(st, vertexp, &chain_term, &dist);
        depth = get_depth(st, vertexp, dist, chain_term);
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



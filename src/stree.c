#include "stree.h"


static Uint* head_suffix_link(STree *st)
{
    VertexP root = st->is.fst;
    if(st->hd.d == 1) {
        return root;
    } else {
        return root + SUFFIX_LINK(st->hd.v);
    }
}


bool base_is_vertex(STree *st)
{
    return st->hd.l.end == NULL;
}


bool is_head_old(STree *st)
{
    return base_is_vertex(st);
}


bool tail_at_lastchar(STree *st)
{
    return st->tl == text.lst;
}


bool head_is_root(STree *st)
{
    return st->hd.d == 0;
}


Uint get_headpos(STree *st, VertexP v, Uint dist, VertexP chain_term)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        return st->ls.nxt_ind - dist;
    } else {
        if (IS_LARGE(*v)) {
            return HEADPOS(v);
        } else {
            return HEADPOS(chain_term) - dist;
        }
    }
}


Uint get_depth(STree *st, VertexP v, Uint dist, VertexP chain_term)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        return st->c_depth  + dist;
    } else {
        if (IS_LARGE(*v)) {
            return DEPTH(v);
        } else {
            return DEPTH(chain_term) + dist;
        }
    }
}


void set_head_to_suffixlink(STree *st)
{
    if(IS_LARGE(*st->hd.v)) {
        st->hd.v = head_suffix_link(st);
    } else {
        st->hd.v += SMALL_VERTEXSIZE;
    }
    st->hd.d--;
}


void set_dist_and_chainterm(STree *st, VertexP v, VertexP *end, Uint *dist)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        *dist = 1 + (st->is.nxt - v) / SMALL_VERTEXSIZE;
    } else if (IS_SMALL(*v)) {
            *dist = DISTANCE(v);
            VertexP chain_term = v + SMALL_VERTEXSIZE * *dist;
            *end   = chain_term;
    }
}

#include "stree.h"

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

Uint get_headpos(STree *st, Uint *v, Uint **chainend, Uint distance)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        return st->ls.nxt_ind - distance;
    } else {
        if(IS_LARGE(*(v))) {
            return HEADPOS(v);
        } else {
            return HEADPOS(*chainend) - distance;
        }
    }
}

Uint get_depth(STree *st, Uint *v, Uint distance, Uint **chainend)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        return st->c_depth  + distance;
    } else {
        if(IS_LARGE(*v)) {
            return DEPTH(v);
        } else {
            return DEPTH(*chainend) + distance;
        }
    }
}

static Uint* suffix_link(STree *st)
{
    Uint *fst = st->is.fst;
    if(st->hd.d == 1) {
        return fst;
    } else {
        return fst + SUFFIX_LINK(st->hd.v);
    }
}

void follow_link(STree *st)
{
    if(IS_LARGE(*st->hd.v)) {
        st->hd.v = suffix_link(st);
    } else {
        st->hd.v += SMALL_VERTEXSIZE;
    }
    st->hd.d--;
}


void set_dist_and_chainend(STree *st, VertexP v, VertexP *end, Uint *dist)
{
    if(st->chain.fst != NULL && v >= st->chain.fst) {
        *dist = 1 + (st->is.nxt - v) / SMALL_VERTEXSIZE;
    } else if (IS_SMALL(*v)) {
            *dist = DISTANCE(v);
            *end   = CHAIN_END(v, *dist);
    }
}



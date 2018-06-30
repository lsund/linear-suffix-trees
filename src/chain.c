#include "chain.h"

void append_chain(STree *st)
{
    if (!st->chain.fst) {
        init_chain(st);
    }
    st->chain.size     += 1;
    st->is.nxt     += SMALL_VERTEXSIZE;
    st->is.nxt_ind += SMALL_VERTEXSIZE;
}

void update_chain(STree *st, Uint *vertexp, Uint **chainend, Uint *distance)
{
    if(st->chain.fst != NULL && vertexp >= st->chain.fst) {
        *distance = 1 + (st->is.nxt - vertexp) / SMALL_VERTEXSIZE;
    } else {
        if(IS_SMALL(*(vertexp))) {
            *distance = DISTANCE(vertexp);
            *chainend   = CHAIN_END(vertexp, *distance);
        }
    }
}

void reset_chain(STree *st)
{
    st->chain.size = 0;
    st->chain.fst = NULL;
}

void init_chain(STree *st)
{
    st->chain.fst = st->is.nxt;
}

void set_chain_distances(STree *st)
{
    Uint distance;

    SUFFIX_LINK(st->is.nxt) = REF_TO_INDEX(st->head.vertex);
    if (st->chain.size > 0) {
        VertexP prev = st->is.nxt;
        for(distance = 1; distance <= st->chain.size; distance++) {
            prev -= SMALL_VERTEXSIZE;
            DISTANCE(prev) = distance;
            *prev = MAKE_SMALL(*prev);
        }
        reset_chain(st);
    }
    st->is.nxt += LARGE_VERTEXSIZE;
    st->is.nxt_ind += LARGE_VERTEXSIZE;
}


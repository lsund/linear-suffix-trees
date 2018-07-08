#include "chain.h"


static void init_chain(STree *st)
{
    st->chain.fst = st->is.nxt;
}


///////////////////////////////////////////////////////////////////////////////
// Public API


void reset_chain(STree *st)
{
    st->chain.size = 0;
    st->chain.fst = NULL;
}


void grow_chain(STree *st)
{
    if (!st->chain.fst) {
        init_chain(st);
    }
    st->chain.size     += 1;
    st->is.nxt     += SMALL_VERTEXSIZE;
    st->is.nxt_ind += SMALL_VERTEXSIZE;
}


void finalize_chain(STree *st)
{
    Uint dist;

    SUFFIX_LINK(st->is.nxt) = REF_TO_INDEX(st->head.vertex);
    if (st->chain.size > 0) {
        VertexP prev = st->is.nxt;
        for(dist = 1; dist <= st->chain.size; dist++) {
            prev -= SMALL_VERTEXSIZE;
            DISTANCE(prev) = dist;
            *prev = MAKE_SMALL(*prev);
        }
        reset_chain(st);
    }
    st->is.nxt += LARGE_VERTEXSIZE;
    st->is.nxt_ind += LARGE_VERTEXSIZE;
}


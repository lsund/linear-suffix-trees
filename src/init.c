#include "init.h"

void init(STree *st)
{
    Uint i;

    st->is.size = START_ALLOCSIZE;

    st->ls.fst = realloc(NULL, sizeof(Uint) * textlen + 2);
    st->is.fst = realloc(NULL, sizeof(Uint) * st->is.size);

    for(i=0; i<LARGE_VERTEXSIZE; i++) {
        st->is.fst[i] = 0;
    }

    st->rootchildren = realloc(NULL, sizeof(Uint) *  MAX_CHARS + 1);
    for(Uint *child= st->rootchildren; child<=st->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    st->tail = text;

    Uint last = st->is.size - LARGE_VERTEXSIZE;
    st->allocated = st->is.fst + last;

    // Inner
    st->is.nxt = st->is.fst;
    st->is.nxt_ind = 0;
    DEPTH(st->is.nxt) = 0;
    HEADPOS(st->is.nxt) = 0;
    SET_CHILD(st->is.nxt, MAKE_LEAF(0));
    SIBLING(st->is.nxt) = 0;

    // Head
    st->head.vertex    = st->is.fst;
    st->head.label.end = NULL;
    st->head.depth     = 0;

    SET_ROOTCHILD(*text, MAKE_LEAF(0));

    st->ls.fst[0]          = 0;
    st->ls.nxt_ind          = LEAF_VERTEXSIZE;
    st->ls.nxt              = st->ls.fst + LEAF_VERTEXSIZE;
    st->is.nxt               = st->is.fst + LARGE_VERTEXSIZE;
    st->is.nxt_ind           = LARGE_VERTEXSIZE;
    st->split.child       = UNDEF;
    st->split.left = UNDEF;
    st->chain.size               = 0;
    st->chain.fst              = NULL;


}

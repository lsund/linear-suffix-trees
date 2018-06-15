#include "init.h"

static void reset_chain(STree *st)
{
    st->chain.size = 0;
    st->chain.first = NULL;
}

void init_chain(STree *st)
{
    st->chain.first = st->inner.next;
}

void set_chain_distances(STree *st)
{
    Uint distance;

    SUFFIX_LINK(st->inner.next) = REF_TO_INDEX(st->head.origin);
    if (st->chain.size > 0) {
        VertexP prev = st->inner.next;
        for(distance = 1; distance <= st->chain.size; distance++) {
            prev -= SMALL_VERTEXSIZE;
            DISTANCE(prev) = distance;
            *prev = MAKE_SMALL(*prev);
        }
        reset_chain(st);
    }
    st->inner.next += LARGE_VERTEXSIZE;
    st->inner.next_ind += LARGE_VERTEXSIZE;
}

void init(STree *st)
{
    Uint i;

    st->inner.size = START_ALLOCSIZE;

    st->leaves.first = realloc(NULL, sizeof(Uint) * textlen + 2);
    st->inner.first = realloc(NULL, sizeof(Uint) * st->inner.size);

    for(i=0; i<LARGE_VERTEXSIZE; i++) {
        st->inner.first[i] = 0;
    }

    st->rootchildren = realloc(NULL, sizeof(Uint) *  MAX_CHARS + 1);
    for(Uint *child= st->rootchildren; child<=st->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    st->tail = text;

    Uint last = st->inner.size - LARGE_VERTEXSIZE;
    st->allocated = st->inner.first + last;

    // Inner
    st->inner.next = st->inner.first;
    st->inner.next_ind = 0;
    DEPTH(st->inner.next) = 0;
    HEADPOS(st->inner.next) = 0;
    SET_CHILD(st->inner.next, MAKE_LEAF(0));
    SIBLING(st->inner.next) = 0;

    // Head
    st->head.origin    = st->inner.first;
    st->head.label.end = NULL;
    st->head.depth     = 0;

    SET_ROOTCHILD(*text, MAKE_LEAF(0));

    st->leaves.first[0]          = 0;
    st->leaves.next_ind          = LEAF_VERTEXSIZE;
    st->leaves.next              = st->leaves.first + LEAF_VERTEXSIZE;
    st->inner.next               = st->inner.first + LARGE_VERTEXSIZE;
    st->inner.next_ind           = LARGE_VERTEXSIZE;
    st->splitvertex.origin       = UNDEF;
    st->splitvertex.left_sibling = UNDEF;
    st->chain.size               = 0;
    st->chain.first              = NULL;


}

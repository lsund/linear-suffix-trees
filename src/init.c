#include "init.h"

static void reset_chain(STree *stree)
{
    stree->chain.size = 0;
    stree->chain.first = NULL;
}

void init_chain(STree *stree)
{
    stree->chain.first = stree->inner.next;
}

void set_chain_distances(STree *stree)
{
    Uint distance;

    SUFFIX_LINK(stree->inner.next) = REF_TO_INDEX(stree->head.origin);
    if (stree->chain.size > 0) {
        VertexP prev = stree->inner.next;
        for(distance = 1; distance <= stree->chain.size; distance++) {
            prev -= SMALL_VERTEXSIZE;
            DISTANCE(prev) = distance;
            *prev = MAKE_SMALL(*prev);
        }
        reset_chain(stree);
    }
    stree->inner.next += LARGE_VERTEXSIZE;
    stree->inner.next_ind += LARGE_VERTEXSIZE;
}

void init(STree *stree)
{
    Uint i;

    stree->inner.size = START_ALLOCSIZE;
    stree->leaves.first = ALLOC(NULL, Uint, textlen + 2);

    stree->inner.first = ALLOC(NULL, Uint, stree->inner.size);
    for(i=0; i<LARGE_VERTEXSIZE; i++) {
        stree->inner.first[i] = 0;
    }

    stree->rootchildren = ALLOC(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    stree->tail = text;

    Uint last = stree->inner.size - LARGE_VERTEXSIZE;
    stree->allocated = stree->inner.first + last;

    // Inner
    stree->inner.next = stree->inner.first;
    stree->inner.next_ind = 0;
    DEPTH(stree->inner.next) = 0;
    HEADPOS(stree->inner.next) = 0;
    SET_CHILD(stree->inner.next, MAKE_LEAF(0));
    SIBLING(stree->inner.next) = 0;

    // Head
    stree->head.origin    = stree->inner.first;
    stree->head.label.end = NULL;
    stree->head.depth     = 0;

    SET_ROOTCHILD(*text, MAKE_LEAF(0));

    stree->leaves.first[0]          = 0;
    stree->leaves.next_ind          = LEAF_VERTEXSIZE;
    stree->leaves.next              = stree->leaves.first + LEAF_VERTEXSIZE;
    stree->inner.next               = stree->inner.first + LARGE_VERTEXSIZE;
    stree->inner.next_ind           = LARGE_VERTEXSIZE;
    stree->splitvertex.origin       = UNDEF;
    stree->splitvertex.left_sibling = UNDEF;
    stree->chain.size               = 0;
    stree->chain.first              = NULL;


}

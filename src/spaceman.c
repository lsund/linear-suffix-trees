#include "spaceman.h"

static Uint *new_allocbound(STree *stree)
{
    return stree->inner.first + stree->inner.size - LARGE_VERTEXSIZE;
}

// Allocate space for branch vertices
void allocate_inner_vertices(STree *stree)
{
    if(stree->inner.next >= stree->allocated) {

        stree->inner.size += EXTRA_ALLOCSIZE;

        Uint chain;
        if(stree->chain.first != NULL) {
            chain = REF_TO_INDEX(stree->chain.first);
        }

        Uint head = REF_TO_INDEX(stree->head.origin);
        Uint size = stree->inner.size;

        stree->inner.first = realloc(stree->inner.first, sizeof(Uint) * size);
        stree->inner.next  = next_inner(stree);
        stree->head.origin = stree->inner.first + head;

        if(stree->chain.first != NULL) {
            stree->chain.first = stree->inner.first + chain;
        }
        stree->allocated = new_allocbound(stree);
    }
}


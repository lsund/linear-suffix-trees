#include "spaceman.h"

static Uint *new_allocbound(STree *st)
{
    return st->inner.first + st->inner.size - LARGE_VERTEXSIZE;
}

// Allocate space for branch vertices
void allocate_inner_vertices(STree *st)
{
    if(st->inner.next >= st->allocated) {

        st->inner.size += EXTRA_ALLOCSIZE;

        Uint chain;
        if(st->chain.first != NULL) {
            chain = REF_TO_INDEX(st->chain.first);
        }

        Uint head = REF_TO_INDEX(st->head.origin);
        Uint size = st->inner.size;

        st->inner.first = realloc(st->inner.first, sizeof(Uint) * size);
        st->inner.next  = get_next_inner(st);
        st->head.origin = st->inner.first + head;

        if(st->chain.first != NULL) {
            st->chain.first = st->inner.first + chain;
        }
        st->allocated = new_allocbound(st);
    }
}


#include "spaceman.h"

static Uint *new_allocbound(STree *st)
{
    return st->is.fst + st->is.size - LARGE_VERTEXSIZE;
}

// Allocate space for branch vertices
void allocate_inner_vertices(STree *st)
{
    if(st->is.nxt >= st->allocated) {

        st->is.size += EXTRA_ALLOCSIZE;

        Uint chain;
        if(st->chain.fst != NULL) {
            chain = REF_TO_INDEX(st->chain.fst);
        }

        Uint head = REF_TO_INDEX(st->head.origin);
        Uint size = st->is.size;

        st->is.fst = realloc(st->is.fst, sizeof(Uint) * size);
        st->is.nxt  = get_nxt_inner(st);
        st->head.origin = st->is.fst + head;

        if(st->chain.fst != NULL) {
            st->chain.fst = st->is.fst + chain;
        }
        st->allocated = new_allocbound(st);
    }
}

#include "spaceman.h"

static Uint *new_allocbound(STree *st)
{
    return st->is.fst + st->is.size - LARGE_VERTEXSIZE;
}


void allocate_inner_vertices(STree *st)
{
    // If new space is needed...
    if(st->is.nxt >= st->is.alloc) {

        st->is.size += EXTRA_ALLOCSIZE;

        // Get the first vertex in the current chain
        Vertex chain;
        if(st->chain.fst != NULL) {
            chain = REF_TO_INDEX(st->chain.fst);
        }

        Uint head_index = REF_TO_INDEX(st->hd.v);
        Uint size = st->is.size;
        st->is.fst = realloc(st->is.fst, sizeof(Uint) * size);
        st->is.nxt  = st->is.fst + st->is.nxt_ind;
        st->hd.v = st->is.fst + head_index;

        // Set the chain again, since the address may change
        if(st->chain.fst != NULL) {
            st->chain.fst = st->is.fst + chain;
        }

        st->is.alloc = new_allocbound(st);
    }
}

#include "location.h"

Text text;

void init_location(Uint *v, Uint hd, Uint d, Loc *loc)
{
    loc->nxt    = v;
    loc->s      = hd;
    loc->d      = d;
    loc->rem    = 0;
    loc->isleaf = false;
}


void make_loc(STree *st, Uint s, Uint plen, Loc *loc, Wchar *fst)
{
    loc->s       = s;
    loc->d       = plen;
    loc->prev    = st->is.fst;
    loc->edgelen = text.len - s + 1;
    loc->rem     = loc->edgelen - plen;
    loc->fst     = fst;
    loc->nxt     = st->ls.fst + s;
}



void update_loc(
        VertexP nxt,
        Uint s,
        Uint plen,
        Wchar *fst,
        Uint d,
        Uint edgelen,
        Loc *loc
    )
{
    loc->s       = s;
    loc->d       = d + plen;
    loc->prev    = loc->nxt;
    loc->edgelen = edgelen;
    loc->rem     = loc->edgelen - plen;
    loc->fst     = fst;
    loc->nxt     = nxt;
}

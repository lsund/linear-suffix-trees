#include "location.h"


void init_location(Uint *nxt, Uint str, Uint d, Loc *loc)
{
    loc->nxt    = nxt;
    loc->str      = str;
    loc->d      = d;
    loc->rem    = 0;
    loc->isleaf = false;
}

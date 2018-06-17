#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "streedef.h"

Uint max(Uint a, Uint b);

Pattern make_patt(Wchar *start, Wchar *end);

Uint lcp(Pattern patt1, Pattern patt2);

Pattern patt_inc(Pattern patt, Uint n);

#endif

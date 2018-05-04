#ifndef UTIL_H
#define UTIL_H

#include "types.h"

Uint lcp(Wchar *start1, Wchar *end1, Wchar *start2, Wchar *end2);

Uint max(Uint a, Uint b);

Pattern make_patt(Wchar *start, Wchar *end);

Uint lcp_patt(Pattern patt1, Pattern patt2);

#endif

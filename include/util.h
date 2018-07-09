#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "streedef.h"
#include "clock.h"

// Bytes into megabytes.
#define MEGABYTES(V)  ((double) (V)/((UINT(1) << 20) - 1))

// The longest common prefix of the two patterns x and y.
Uint lcp(Pattern x, Pattern y);

// Create a pattern, given a start and an end pointer.
Pattern make_patt(Wchar *start, Wchar *end);

// Increment the start pointer of a pattern by `n`.
Pattern patt_inc(Pattern patt, Uint n);

// The smaller of the numbers a and b.
Uint min(const Uint a, const Uint b);

// The larger of the numbers a and b.
Uint max(const Uint a, const Uint b);

#endif

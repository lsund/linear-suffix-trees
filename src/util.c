
#include "util.h"


Uint lcp(Pattern patt1, Pattern patt2)
{
    register Wchar *probe1 = patt1.start, *probe2 = patt2.start;

    while(probe1 <= patt1.end && probe2 <= patt2.end && *probe1 == *probe2) {
        probe1++;
        probe2++;
    }
    return (Uint) (probe1 - patt1.start);
}


Uint max(Uint a, Uint b)
{
    return a > b ? a : b;
}


Uint min(const Uint a, const Uint b)
{
    return a < b ? a : b;
}


Pattern make_patt(Wchar *start, Wchar *end)
{
    Pattern ret;
    ret.start = start;
    ret.end = end;
    return ret;
}


Pattern patt_inc(Pattern patt, Uint n)
{
    Pattern ret;
    ret.start = patt.start + n;
    ret.end = patt.end;
    return ret;
}


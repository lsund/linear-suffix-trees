
#include "util.h"


Uint lcp(Pattern x, Pattern y)
{
    register Wchar *px = x.start, *py = y.start;

    while(px <= x.end && py <= y.end && *px == *py) {
        px++;
        py++;
    }
    return (Uint) (px - x.start);
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


Uint max(const Uint a, const Uint b)
{
    return a > b ? a : b;
}


Uint min(const Uint a, const Uint b)
{
    return a < b ? a : b;
}

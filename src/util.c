
#include "util.h"

Uint lcp(Wchar *start1, Wchar *end1, Wchar *start2, Wchar *end2)
{
    register Wchar *ptr1 = start1, *ptr2 = start2;

    while(ptr1 <= end1 && ptr2 <= end2 && *ptr1 == *ptr2) {
        ptr1++;
        ptr2++;
    }
    return (Uint) (ptr1 - start1);
}

Uint max(Uint a, Uint b)
{
    return a > b ? a : b;
}




#include "util.h"

Uint lcp(wchar_t *start1,wchar_t *end1,wchar_t *start2,wchar_t *end2)
{
    register wchar_t *ptr1 = start1, *ptr2 = start2;

    while(ptr1 <= end1 && ptr2 <= end2 && *ptr1 == *ptr2) {
        ptr1++;
        ptr2++;
    }
    return (Uint) (ptr1 - start1);
}


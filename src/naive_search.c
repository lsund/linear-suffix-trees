
#include "naive_search.h"

Uint textlen;

Uchar *wtext;


// Naively search `wtext` for the pattern beginning at address `start`, ending
// at address `end`.
bool naive_search(Uchar *start, Uchar *end)
{
    Uint m = (Uint) (end - start);
    Uchar *pattern = start;
    Uint j = 0;
    Uint k;

    for (Uint i = 0; i < textlen; i++) {
        k = i;
        for (j = 0; j < m; j++) {
            if (pattern[j] == wtext[k]) {
                k++;
            } else {
                break;
            }
        }
        if (j == m) {
            return true;
        }
    }
    return false;
}

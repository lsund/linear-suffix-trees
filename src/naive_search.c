
#include "naive_search.h"
#include "externs.h"

Uint textlen;
Wchar *text;

// Naively search `wtext` for the pattern beginning at address `start`, ending
// at address `end`.
bool naive_search(Wchar *start, Wchar *end)
{
    Uint m = (Uint) (end - start);
    Wchar *pattern = start;
    Uint j = 0;
    Uint k;

    for (Uint i = 0; i < textlen; i++) {
        k = i;
        for (j = 0; j < m; j++) {
            if (pattern[j] == text[k]) {
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

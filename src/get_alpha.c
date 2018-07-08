#include "get_alpha.h"

Text text;

Uint get_characters()
{
    Wchar alpha[MAX_CHARS + 1];
    Uint counts[MAX_CHARS + 1] = {0};
    Wchar *curr_text;

    for (curr_text = text.fst; curr_text < text.fst + text.len; curr_text++) {
        counts[(Uint) *curr_text]++;
    }

    Uint i, j;
    for (j = 0, i = 0; i <= MAX_CHARS; i++) {
        if (counts[i] > 0) {
            alpha[j++] = (Wchar) i;
        }
    }
    return j;
}

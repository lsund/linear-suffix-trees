#ifndef EXTERNS_H
#define EXTERNS_H

#include "types.h"
#include "config.h"
#include "streedef.h"

// The label for a incoming edge to a vertex wu can be obtained by dropping
// depth(w) characters of wu.
#define LABEL(O)          text.fst + (O)

// A Text represents the text or string used to build a suffix tree.
typedef struct text {
    // Points to the start of the allocated block, the first character in the
    // text.
    Wchar *fst;
    // Points to the last of the allocated black, the last character in the
    // text.
    Wchar *lst;
    // The number of characters in the text.
    Uint len;
    // The distinct characters in alphabetical order.
    Uint asize;
} Text;

extern Text text;

#endif

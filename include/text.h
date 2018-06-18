#ifndef EXTERNS_H
#define EXTERNS_H

#include "types.h"
#include "basedef.h"
#include "streedef.h"

typedef struct text {
    Wchar *fst;                // points to the input string
    Wchar *lst;               // points to the position of the \(\$\)-symbol
    Uint len;                      // the length of the input string
} Text;

extern Text text;

#endif

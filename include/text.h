#ifndef EXTERNS_H
#define EXTERNS_H

#include "types.h"
#include "basedef.h"
#include "streedef.h"

typedef struct text {
    Wchar *content;                // points to the input string
    Wchar *sentinel;               // points to the position of the \(\$\)-symbol
    Uint len;                      // the length of the input string
} Text;

extern Text text;

#endif

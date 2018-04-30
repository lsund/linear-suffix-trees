#ifndef EXTERNS_H
#define EXTERNS_H

#define MAX_PATTERNS 10000

#include "types.h"

extern wchar_t *text;

extern wchar_t *sentinel, characters[UCHAR_MAX + 1];
extern Uint     root_children[UCHAR_MAX + 1],
                textlen,
                *stree,
                alphasize;
#endif

#ifndef EXTERNS_H
#define EXTERNS_H

#include "types.h"
#include "basedef.h"
#include "streedef.h"

extern Wchar *text;

extern Wchar    *sentinel, characters[MAX_CHARS + 1];
extern Uint     root_children[MAX_CHARS + 1], textlen, alphasize;

#endif

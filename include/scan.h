#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "stree.h"
#include "basedef.h"
#include "util.h"

wchar_t *scan(
            STree *stree,
            Location *loc,              // The location of the prefix (out)
            Uint *btptr,                 // Branch reference
            wchar_t *left,               // The start
            wchar_t *right              // The end
        );

#endif

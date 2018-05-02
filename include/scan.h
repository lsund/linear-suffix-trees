#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "stree.h"
#include "basedef.h"
#include "util.h"

Wchar *scan(
            STree *stree,
            Loc *loc,              // The location of the prefix (out)
            Uint *btptr,                 // Branch reference
            Pattern patt
        );

#define MATCHED(X, R, L)        (X) == (Uint) ((R) - (L) + 1)

#endif

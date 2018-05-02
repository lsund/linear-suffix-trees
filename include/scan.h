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
            Wchar *left,               // The start
            Wchar *right              // The end
        );

#endif

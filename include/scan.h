#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "stree.h"

wchar_t *scanprefixfromnodestree(
            Suffixtree *stree,
            Location *loc,              // The location of the prefix (out)
            Bref btptr,                 // Branch reference
            wchar_t *left,               // The start
            wchar_t *right,              // The end
            Uint rescanlength
        );

#endif

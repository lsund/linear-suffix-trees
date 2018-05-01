#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "streetyp.h"

SYMBOL *scanprefixfromnodestree(
            Suffixtree *stree,
            Location *loc,              // The location of the prefix (out)
            Bref btptr,                 // Branch reference
            SYMBOL *left,               // The start
            SYMBOL *right,              // The end
            Uint rescanlength
        );

#endif

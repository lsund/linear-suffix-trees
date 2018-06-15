
#ifndef DFS_H
#define DFS_H

#include "arraydef.h"
#include "spaceman.h"
#include "stree.h"

typedef struct reference {
    Bool toleaf;    // Points to a leaf?
    Uint *address;
} Reference;


Sint stree_dfs(STree *st,Reference *start,
        Sint (*processleaf)(Uint, ArrayUint *), ArrayUint *leaves);


Sint makeleaflist(STree *st, ArrayUint *leaflist, Reference *start);

#endif

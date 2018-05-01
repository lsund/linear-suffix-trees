
#ifndef DFS_H
#define DFS_H

#include "stree.h"

Sint depthfirststree(Suffixtree *stree,Uint * *startnode,
        Sint (*processleaf)(Uint,Uint *,void *),
        Bool (*processbranch1)(Uint *,void *),
        Sint (*processbranch2)(Uint *,void *),
        Bool (*stoptraversal)(void *),void *stopinfo,void *info
    );

#endif

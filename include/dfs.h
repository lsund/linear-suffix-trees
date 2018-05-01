
#ifndef DFS_H
#define DFS_H

#include "streetyp.h"

Sint depthfirststree(Suffixtree *stree,Reference *startnode,
        Sint (*processleaf)(Uint,Bref,void *),
        Bool (*processbranch1)(Bref,void *),
        Sint (*processbranch2)(Bref,void *),
        Bool (*stoptraversal)(void *),void *stopinfo,void *info
    );

#endif

#ifndef STREE_AUX_H
#define STREE_AUX_H

#include "streedata.h"
#include "stree.h"


Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

Uint get_head( STree *stree, Uint *vertexp, Uint *largep, Uint *distance);

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint *largep);


#endif

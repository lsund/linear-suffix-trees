#ifndef ACCESS_H
#define ACCESS_H

#include "stree.h"

void getbranchinfostree(Suffixtree *stree,Uint whichinfo,
                        Branchinfo *branchinfo,Bref btptr);

Uint getmaxtextlenstree(void);

#endif


#include "streehuge.h"

Uint getlargelinkconstruction(Suffixtree *stree)
{
    SYMBOL secondchar;

    DEBUG2(FUNCLEVEL,">%s(%lu)\n",
            __func__,
            (Ulong) BRADDR2NUM(stree,stree->headnode));
    if(stree->headnodedepth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->headnodedepth == 2)  // determine second char of egde
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    return *(stree->headnode+4);
}

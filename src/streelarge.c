
#include "streelarge.h"

void setdepthheadposition(Suffixtree *stree,Uint depth,
        Uint headposition)
{
    DEBUG4(FUNCLEVEL,">%s(%lu)=(depth=%lu,headposition=%lu)\n",
            __func__,
            (Ulong) stree->nextfreebranchnum,
            (Ulong) depth,
            (Ulong) headposition);

    if(ISSMALLDEPTH(depth))
    {
        *(stree->nextfreebranch+2) = depth | SMALLDEPTHMARK;
    } else
    {
        *(stree->nextfreebranch+2) = depth;
    }
    *(stree->nextfreebranch+3) = headposition;
}

void setsuffixlink(Suffixtree *stree,Uint slink)
{
    Uint slinkhalf = slink >> 1;

    DEBUG3(FUNCLEVEL,">%s(%lu)=%lu\n",
            __func__,
            (Ulong) stree->nextfreebranchnum,
            (Ulong) slink);
    *(stree->setlink) = (*(stree->setlink) & EXTRAPATT) | (slink | NILBIT);
    if(ISSMALLDEPTH(stree->currentdepth))
    {
        *(stree->nextfreebranch + 2)
            |= ((slinkhalf << SMALLDEPTHBITS) & LOWERLINKPATT);
        if(stree->nextfreebranchnum & (~LOWERLINKSIZE))
        {
            *(stree->nextfreebranch + 3)
                |= ((slinkhalf << SHIFTMIDDLE) & MIDDLELINKPATT);
            if(stree->nextfreebranchnum & HIGHERSIZE)
            {
                stree->leaftab[stree->nextfreeleafnum-1] |=
                    ((slinkhalf << SHIFTHIGHER) & EXTRAPATT);
            }
        }
    }
}

Uint getlargelinkconstruction(Suffixtree *stree)
{
    SYMBOL secondchar;
    Uint succ, slinkhalf, headnodenum;

    DEBUG2(FUNCLEVEL,">%s(%lu)\n",
            __func__,
            (Ulong) BRADDR2NUM(stree,stree->headnode));
    DEBUGCODE(1,stree->largelinks++);
    if(stree->headnodedepth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->headnodedepth == 2)  // determine second char of egde
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else
        {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    if(ISSMALLDEPTH(stree->headnodedepth))  // retrieve link in constant time
    {
        slinkhalf = (*(stree->headnode+2) & LOWERLINKPATT) >> SMALLDEPTHBITS;
        headnodenum = BRADDR2NUM(stree,stree->headnode);
        if(headnodenum & (~LOWERLINKSIZE))
        {
            slinkhalf |= ((*(stree->headnode+3) & MIDDLELINKPATT) >> SHIFTMIDDLE);
            if(headnodenum & HIGHERSIZE)
            {
                slinkhalf
                    |= ((stree->leaftab[GETHEADPOS(stree->headnode)] & EXTRAPATT)
                            >> SHIFTHIGHER);
            }
        }
        return slinkhalf << 1;
    }
    succ = stree->onsuccpath;
    DEBUGCODE(1,stree->largelinklinkwork++);
    while(!NILPTR(succ))   // linear retrieval of suffix link
    {
        DEBUGCODE(1,stree->largelinkwork++);
        if(ISLEAF(succ))
        {
            succ = LEAFBROTHERVAL(stree->leaftab[GETLEAFINDEX(succ)]);
        } else
        {
            succ = GETBROTHER(stree->branchtab + GETBRANCHINDEX(succ));
        }
        DEBUGCODE(1,stree->largelinkwork++);
    }
    return succ & MAXINDEX;   // get only the index
}

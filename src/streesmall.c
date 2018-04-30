
#include "streesmall.h"

// Initializing and Retrieving Headpositions, Depth, and Suffixlinks

//
//  We have three functions to initialize and retrieve head positions, depth, and
//  suffix links. The implementation depends on the bit layout.
//  \begin{enumerate}
//  \item
//  The function \emph{setdepthnum} stores the \emph{depth} and the
//  \emph{head position} of a new large node.
//  \item
//  The function \emph{setsuffixlink} stores the \emph{suffixlink}
//  of a new large node.
//  \item
//  The function \emph{getlargelinkconstruction} retrieves the \emph{suffixlink}
//  of a large node, which is referenced by \emph{headposition}.
//  \end{enumerate}
//  */

void setdepthheadposition(Suffixtree *stree,Uint depth, Uint headposition)
{
    DEBUG4(FUNCLEVEL,">%s(%lu)=(depth=%lu,headposition=%lu)\n",
            __func__,
            (Ulong) stree->nextfreebranchnum,
            (Ulong) depth,
            (Ulong) headposition);
    DEBUGCODE(1,stree->maxset = stree->nextfreebranch + 2);
    if(ISSMALLDEPTH(depth))
    {
        *(stree->nextfreebranch+1) |= SMALLDEPTHMARK;
    } else
    {
        *(stree->nextfreebranch)
            = (*(stree->nextfreebranch) & (MAXINDEX | LARGEBIT)) |
            ((depth << 11) & (7 << 29));
        *(stree->nextfreebranch+1)
            = (*(stree->nextfreebranch+1) & (MAXINDEX | NILBIT)) |
            ((depth << 14) & (127 << 25));
    }
    *(stree->nextfreebranch+2) = (depth << 21) | headposition;
}

void setsuffixlink(Suffixtree *stree,Uint slink)
{
    DEBUG3(FUNCLEVEL,">%s(%lu)=%lu\n",
            __func__,
            (Ulong) stree->nextfreebranchnum,
            (Ulong) slink);
    *(stree->setlink) = (*(stree->setlink) & (255 << 24)) | (slink | NILBIT);
    if(ISSMALLDEPTH(stree->currentdepth))
    {
        *(stree->nextfreebranch+1) |= (slink << 25);
        if(stree->nextfreebranchnum & (~((1 << 7) - 1)))
        {
            *(stree->nextfreebranch) |= ((slink << 17) & (255 << 24));
            if(stree->nextfreebranchnum & (~((1 << 15) - 1)))
            {
                stree->leaftab[stree->nextfreeleafnum-1] |=
                    ((slink << 9) & (255 << 24));
            }
        }
    }
}

Uint getlargelinkconstruction(Suffixtree *stree)
{
    SYMBOL secondchar;
    Uint succ, slink, headnodenum;

    DEBUG2(FUNCLEVEL,">%s(%lu)\n",
            __func__,
            (Ulong) BRADDR2NUM(stree,stree->headnode));

    DEBUGCODE(1,stree->largelinks++);
    if(stree->headnodedepth == 1)
    {
        return 0;               // link refers to the root
    }
    if(stree->headnodedepth == 2)   // determine second character of edge
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else
        {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        // this leads to the suffix link node
        return GETBRANCHINDEX(stree->rootchildren[(Uint) secondchar]);
    }
    if(ISSMALLDEPTH(stree->headnodedepth))   // retrieve link in constant time
    {
        slink = *(stree->headnode+1) >> 25;
        headnodenum = BRADDR2NUM(stree,stree->headnode);
        if(headnodenum & (~((1 << 7) - 1)))
        {
            slink |= ((*(stree->headnode) & (255 << 24)) >> 17);
            if(headnodenum & (~((1 << 15) - 1)))
            {
                slink |= ((stree->leaftab[GETHEADPOS(stree->headnode)]
                            & (255 << 24)) >> 9);
            }
        }
        return slink;
    }
    succ = stree->onsuccpath;   // start at node on successor path
    DEBUGCODE(1,stree->largelinklinkwork++);
    while(!NILPTR(succ))        // linear retrieval of suffix links
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

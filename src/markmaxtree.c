
#include "construct.h"


#define COMPLETELARGESECOND\
            SETIBIT(stree->nonmaximal,stree->nextfreeleafnum);\
            completelarge(stree)

#define PROCESSHEAD          /* Nothing */
#define CHECKSTEP            /* Nothing */
#define FINALPROGRESS        /* Nothing */

Sint constructmarkmaxstree(Suffixtree *stree,wchar_t *text,Uint textlen)
{

    Uint distance, headposition = 0, *largeptr,
         tabsize = 1 + DIVWORDSIZE(textlen+1), *tabptr;
    stree->nonmaximal = ALLOC(NULL,Uint,tabsize);
    for(tabptr = stree->nonmaximal; tabptr < stree->nonmaximal + tabsize; tabptr++)
    {
        *tabptr = 0;
    }

    if (textlen > MAXTEXTLEN) {
        fprintf(stderr, "Text too large");
    }

    initSuffixtree(stree,text,textlen);
    while(stree->tailptr < stree->sentinel ||
            stree->headnodedepth != 0 || stree->headend != NULL)
    {
        CHECKSTEP;
        // case (1): headloc is root
        if(stree->headnodedepth == 0 && stree->headend == NULL)
        {
            (stree->tailptr)++;
            scanprefix(stree);
        } else
        {
            if(stree->headend == NULL)  // case (2.1): headloc is a node
            {
                FOLLOWSUFFIXLINK;
                scanprefix(stree);
            } else               // case (2.2)
            {
                if(stree->headnodedepth == 0) // case (2.2.1): at root: do not use links
                {
                    if(stree->headstart == stree->headend)  // rescan not necessary
                    {
                        stree->headend = NULL;
                    } else
                    {
                        (stree->headstart)++;
                        rescan(stree);
                    }
                } else
                {
                    FOLLOWSUFFIXLINK;    // case (2.2.2)
                    rescan(stree);
                }
                // case (2.2.3): headloc is a node
                if(stree->headend == NULL)
                {
                    SETSUFFIXLINK(BRADDR2NUM(stree,stree->headnode));
                    GETONLYHEADPOS(headposition,stree->headnode);
                    SETIBIT(stree->nonmaximal,headposition);
                    completelarge(stree);
                    scanprefix(stree);
                } else {
                    if(stree->smallnotcompleted == MAXDISTANCE)  // artifical large node
                    {
                        SETSUFFIXLINK(stree->nextfreebranchnum + LARGEINTS);
                        COMPLETELARGESECOND;
                    } else
                    {
                        if(stree->chainstart == NULL)
                        {
                            stree->chainstart = stree->nextfreebranch;   // start new chain
                        }
                        (stree->smallnotcompleted)++;
                        (stree->nextfreebranch) += SMALLINTS;      // case (2.2.4)
                        (stree->nextfreebranchnum) += SMALLINTS;
                        stree->smallnode++;
                    }
                }
            }
        }

      PROCESSHEAD;

      if(stree->headend == NULL)
      {
          insertleaf(stree);  // case (a)
      } else
      {
          insertbranchnode(stree);  // case (b)
      }
  }
  stree->chainstart = NULL;
  linkrootchildren(stree);

  //\Ignore{

  //}
  FINALPROGRESS;
  return 0;
}

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS



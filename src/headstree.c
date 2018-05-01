
#include "construct.h"

Sint constructheadstree(Suffixtree *stree,wchar_t *text,Uint
        textlen,void(*processhead)(Suffixtree *,Uint,void *),void
        *processheadinfo)
{

    stree->nonmaximal = NULL;

  if (textlen > MAXTEXTLEN) {
      fprintf(stderr, "Text too large");
  }

  initSuffixtree(stree,text,textlen);
  while(stree->tailptr < stree->sentinel ||
        stree->headnodedepth != 0 || stree->headend != NULL)
  {
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
        if(stree->headend == NULL)  // case (2.2.3): headloc is a node
        {
          SETSUFFIXLINK(BRADDR2NUM(stree,stree->headnode));
            completelarge(stree);
          scanprefix(stree);
        } else
        {
          if(stree->smallnotcompleted == MAXDISTANCE)  // artifical large node
          {
            SETSUFFIXLINK(stree->nextfreebranchnum + LARGEINTS);
            completelarge(stree);
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

    processhead(stree,stree->nextfreeleafnum, processheadinfo);

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

  return 0;
}


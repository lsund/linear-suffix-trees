
#include "construct.h"

Sint constructheadstree(Suffixtree *stree,wchar_t *text,Uint
        textlen,void(*processhead)(Suffixtree *,Uint,void *),void
        *processheadinfo)
{

    stree->nonmaximal = NULL;

  if (textlen > MAXTEXTLEN) {
      fprintf(stderr, "Text too large");
  }

  DEBUGCODE(3,showvalues());

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
            DEBUGCODE(1,stree->artificial++);
            DEBUG1(3,"#artifical large node %lu\n",
                      (Ulong) stree->nextfreebranchnum);
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
    DEBUGCODE(5,showtable(stree,False));
  }
  stree->chainstart = NULL;
  linkrootchildren(stree);

//\Ignore{

  DEBUG1(2,"#integers for branchnodes %lu\n",
           (Ulong) stree->nextfreebranchnum);
  DEBUG4(2,"#small %lu large %lu textlen %lu all %lu ",
            (Ulong) stree->smallnode,(Ulong) stree->largenode,
            (Ulong) stree->textlen,
            (Ulong) (stree->smallnode+stree->largenode));
  DEBUG1(2,"ratio %f\n",
         (double) (stree->smallnode+stree->largenode)/stree->nextfreeleafnum);
  DEBUG1(2,"#splitleafedge = %lu\n",(Ulong) stree->splitleafedge);
  DEBUG1(2,"#splitinternaledge = %lu\n",(Ulong) stree->splitinternaledge);
  DEBUG1(2,"#insertleafcalls = %lu\n",(Ulong) stree->insertleafcalls);
  DEBUG1(2,"#artificial = %lu\n",(Ulong) stree->artificial);
  DEBUG1(2,"#multiplications = %lu\n",(Ulong) stree->multiplications);
  DEBUGCODE(4,showtable(stree,True));
  DEBUGCODE(3,showstree(stree));
#ifdef DEBUG
  {
    DEBUG3(2,"#largelinks %lu largelinklinkwork %lu largelinkwork %lu ",
              (Ulong) stree->largelinks,
              (Ulong) stree->largelinklinkwork,
              (Ulong) stree->largelinkwork);
    DEBUG2(2,"#ratio1 %.4f ratio2 %.4f\n",
              (double) stree->largelinkwork/stree->largelinks,
              (double) stree->largelinkwork/stree->textlen);
  }
#endif
  DEBUG2(2,"#%6lu %6lu\n",(Ulong) stree->smallnode,
                          (Ulong) stree->largenode);
  DEBUGCODE(2,showspace());
  DEBUGCODE(1,checkstree(stree));

//}
  return 0;
}


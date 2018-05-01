
#include "construct.h"

#define LEASTSHOWPROGRESS 100000
#define NUMOFCALLS 100

#define CONSTRUCT Sint constructprogressstree(Suffixtree *stree,wchar_t *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info)
#define DECLAREEXTRA\
    Uint j = 0, step, nextstep;\
    stree->nonmaximal = NULL;\
    step = textlen/NUMOFCALLS;\
    nextstep = (textlen >= LEASTSHOWPROGRESS) ? step : (textlen+1);\
    if(progress == NULL && textlen >= LEASTSHOWPROGRESS)\
{\
    fprintf(stderr,"# process %lu characters per dot\n",\
            (Ulong) textlen/NUMOFCALLS);\
}

#define COMPLETELARGEFIRST  completelarge(stree)
#define COMPLETELARGESECOND completelarge(stree)
#define PROCESSHEAD          /* Nothing */

#define CHECKSTEP            if(j == nextstep)\
{\
    if(progress == NULL)\
    {\
        if(nextstep == step)\
        {\
            fputc('#',stderr);\
        }\
        fputc('.',stderr);\
        fflush(stdout);\
    } else\
    {\
        progress(nextstep,info);\
    }\
    nextstep += step;\
}\
j++

#define FINALPROGRESS        if(textlen >= LEASTSHOWPROGRESS)\
{\
    if(finalprogress == NULL)\
    {\
        fputc('\n',stderr);\
    } else\
    {\
        finalprogress(info);\
    }\
}

CONSTRUCT
{
  DECLAREEXTRA;

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
        if(stree->headend == NULL)  // case (2.2.3): headloc is a node
        {
          SETSUFFIXLINK(BRADDR2NUM(stree,stree->headnode));
          COMPLETELARGEFIRST;
          scanprefix(stree);
        } else
        {
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

  FINALPROGRESS;
  return 0;
}

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGEFIRST
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS

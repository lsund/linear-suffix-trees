/*
  This file is generated. Do not edit.

  A Library for the Efficient Construction and Application of Suffix Trees

  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef STREEPROTO_H
#define STREEPROTO_H

#ifdef __cplusplus
extern "C" {
#endif

Sint constructstree(Suffixtree *stree,wchar_t *text,Uint textlen);
Sint constructmarkmaxstree(Suffixtree *stree,wchar_t *text,Uint textlen);
Sint constructheadstree(Suffixtree *stree,wchar_t *text,Uint textlen,void(*processhead)(Suffixtree *,Uint,void *),void *processheadinfo);
Sint constructprogressstree(Suffixtree *stree,wchar_t *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info);

void freestree(Suffixtree *stree);
#ifdef __cplusplus
}
#endif

/* access.c */
void getleafinfostree(Suffixtree *stree, Leafinfo *leafinfo, Lref lptr);
void getbranchinfostree(Suffixtree *stree, Uint whichinfo, Branchinfo *branchinfo, Bref btptr);
void getheadstringstree(Suffixtree *stree, String *str);
Uint getmaxtextlenstree(void);
void showpathstree(Suffixtree *stree, Bref bnode, void (*showchar)(wchar_t, void *), void *info);
void rootsucclocationsstree(Suffixtree *stree, ArraySimpleloc *ll);
void succlocationsstree(Suffixtree *stree, unsigned char nosentinel, Simpleloc *loc, ArraySimpleloc *ll);
/* scanpref.c */
wchar_t *scanprefixfromnodestree(Suffixtree *stree, Location *loc, Bref btptr, wchar_t *left, wchar_t *right, Uint rescanlength);
wchar_t *scanprefixstree(Suffixtree *stree, Location *outloc, Location *inloc, wchar_t *left, wchar_t *right, Uint rescanlength);
wchar_t *findprefixpathfromnodestree(Suffixtree *stree, ArrayPathinfo *path, Location *loc, Bref btptr, wchar_t *left, wchar_t *right, Uint rescanlength);
wchar_t *findprefixpathstree(Suffixtree *stree, ArrayPathinfo *path, Location *outloc, Location *inloc, wchar_t *left, wchar_t *right, Uint rescanlength);
/* linkloc.c */
void rescanstree(Suffixtree *stree, Location *loc, Bref btptr, wchar_t *left, wchar_t *right);
void linklocstree(Suffixtree *stree, Location *outloc, Location *inloc);
/* depthtab.c */
void showdepthtab(ArrayUint *dt);
void makedepthtabstree(ArrayUint *depthtab, Suffixtree *stree);
/* ex2leav.c */
unsigned char exactlytwoleavesstree(Suffixtree *stree, PairUint *twoleaves, Bref start);
/* dfs.c */
Sint depthfirststree(Suffixtree *stree, Reference *startnode, Sint (*processleaf)(Uint, Bref, void *), unsigned char (*processbranch1)(Bref, void *), Sint (*processbranch2)(Bref, void *), unsigned char (*stoptraversal)(void *), void *stopinfo, void *info);
Sint makeleaflist(Suffixtree *stree, ArrayUint *leaflist, Reference *start);
/* overmax.c */
void overallstree(Suffixtree *stree, unsigned char skiproot, void (*processnode)(Suffixtree *, Bref, Uint, Uint, void *), void *info);
void overmaximalstree(Suffixtree *stree, void (*processnode)(Suffixtree *, Bref, Uint, Uint, void *), void *info);
/* oversucc.c */
void oversuccsstree(Suffixtree *stree, Bref bnode, void (*processleaf)(Suffixtree *, Uint, void *), void (*processbranch)(Suffixtree *, Bref, void *), void *info);
/* addleafcount.c */
Uint getleafcountstree(Suffixtree *stree, Bref nodeptr);
Sint addleafcountsstree(Suffixtree *stree);
/* iterator.c */
Bref firstbranchingnode(Suffixtree *stree);
Bref nextbranchingnode(Suffixtree *stree, Bref bptr);
Lref firstleaf(Suffixtree *stree);
Lref nextleaf(Suffixtree *stree, Lref lptr);
Reference *firstnode(Suffixtree *stree, Reference *refspace);
Reference *nextnode(Suffixtree *stree, Reference *nref, Reference *refspace);
Reference *firstsucc(Suffixtree *stree, Bref bptr, Reference *refspace);
Reference *rightbrother(Suffixtree *stree, Reference *node);
Reference *firstnodedfs(Suffixtree *stree, DFSstate *dfsstate, Reference *current);
Reference *nextnodedfs(Suffixtree *stree, Reference *current, DFSstate *dfsstate);
/* streedbg.c */
#endif

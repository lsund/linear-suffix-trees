#include "construct.h"

static bool last_suffix(Suffixtree *stree)
{
    return stree->tailptr >= stree->sentinel;
}


static bool head_is_node(Suffixtree *stree)
{
    return stree->headend == NULL;
}


static bool root_depth(Suffixtree *stree)
{
    return stree->headnodedepth == 0;
}


static bool head_is_root(Suffixtree *stree)
{
    return root_depth(stree) && head_is_node(stree);
}


static bool empty_head(Suffixtree *stree)
{
    return stree->headstart == stree->headend;
}


static void insert_vertex(Suffixtree *stree)
{
    if(head_is_node(stree)) {
        insertleaf(stree);
    } else {
        insertbranchnode(stree);
    }
}

Sint constructstree(Suffixtree *stree,wchar_t *text,Uint textlen)
{
    stree->nonmaximal = NULL;

    init(stree,text,textlen);

    while(!last_suffix(stree) || !head_is_root(stree)) {

        // case (1)
        if(head_is_root(stree)) {

            (stree->tailptr)++;
            scanprefix(stree);

        // Case 2
        } else {
            // Case 2.1: Head is node
            if(head_is_node(stree)) {

                FOLLOWSUFFIXLINK;
                scanprefix(stree);

            // Case 2.2
            } else {
                // Case 2.2.1: at root, do not use links
                if(root_depth(stree))
                {
                    // No need to rescan
                    if(empty_head(stree)) {
                        stree->headend = NULL;
                    } else {

                        (stree->headstart)++;
                        rescan(stree);

                    }
                // Case 2.2.2
                } else {
                    FOLLOWSUFFIXLINK;
                    rescan(stree);
                }
                // Case 2.2.3
                if(head_is_node(stree)) {

                    SETSUFFIXLINK(BRADDR2NUM(stree,stree->headnode));
                    completelarge(stree);
                    scanprefix(stree);

                } else {
                    // artificial large node
                    if(stree->smallnotcompleted == MAXDISTANCE) {

                        SETSUFFIXLINK(stree->nextfreebranchnum + LARGEINTS);
                        completelarge(stree);

                    } else {
                        if(stree->chainstart == NULL) {
                            // Start new chain
                            stree->chainstart = stree->nextfreebranch;
                        }
                        (stree->smallnotcompleted)++;
                        (stree->nextfreebranch) += SMALLINTS;      // case (2.2.4)
                        (stree->nextfreebranchnum) += SMALLINTS;
                        stree->smallnode++;
                    }
                }
            }
        }

        insert_vertex(stree);

    }
    stree->chainstart = NULL;
    linkrootchildren(stree);

    return 0;
}

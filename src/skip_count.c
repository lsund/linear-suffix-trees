
#include "skip_count.h"

void skip_count(STree *stree)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar headchar, edgechar;

    Uint *vertexp  = NULL;;
    Uint *chainend = NULL;
    Uint distance = 0;
    if(stree->head_depth == 0)   // head is the root
    {
        headchar = *(stree->headstart);  // headstart is assumed to be not empty
        Uint rootchild = stree->rootchildren[(Uint) headchar];
        if(IS_LEAF(rootchild))   // stop if successor is leaf
        {
            stree->insertnode = rootchild;
            return;
        }
        vertexp = stree->inner.first + LEAF_NUM(rootchild);

        get_chainend(stree, vertexp, &chainend, &distance);
        depth = get_depth(stree, vertexp, distance, &chainend);

        Uint wlen = (stree->headend - stree->headstart + 1);
        if(depth > wlen)    // cannot reach the successor node
        {
            stree->insertnode = rootchild;
            return;
        }
        stree->headnode = vertexp;        // go to successor node
        stree->head_depth = depth;
        if(depth == wlen)             // location has been scanned
        {
            stree->headend = NULL;
            return;
        }
        (stree->headstart) += depth;
    }
    while(True)   // \emph{headnode} is not the root
    {
        headchar = *(stree->headstart);  // \emph{headstart} is assumed to be nonempty
        prevnode = UNDEF;
        Uint headchild = CHILD(stree->headnode);
        while(True)             // traverse the list of successors
        {
            if(IS_LEAF(headchild))   // successor is leaf
            {
                leafindex = LEAF_NUM(headchild);
                edgechar = text[stree->head_depth + leafindex];
                if(edgechar == headchar)    // correct edge found
                {
                    stree->insertnode = headchild;
                    stree->insertprev = prevnode;
                    return;
                }
                prevnode = headchild;
                headchild = stree->leaves.first[leafindex];
            } else   // successor is branch node
            {
                vertexp = stree->inner.first + LEAF_NUM(headchild);

                get_chainend(stree, vertexp, &chainend, &distance);
                head = get_head(stree, vertexp, &chainend, distance);

                edgechar = text[stree->head_depth + head];
                // Correct edge found
                if(edgechar == headchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(vertexp);
            }
        }

        depth = get_depth(stree, vertexp, distance, &chainend);
        edgelen = depth - stree->head_depth;
        Uint wlen = (stree->headend - stree->headstart + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            stree->insertnode = headchild;
            stree->insertprev = prevnode;
            return;
        }
        // go to the successor node
        stree->headnode = vertexp;
        stree->head_depth = depth;
        if(edgelen == wlen) {
            // location is found
            stree->headend = NULL;
            return;
        }
        (stree->headstart) += edgelen;
    }
}

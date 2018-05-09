
#include "skip_count.h"

void skip_count(STree *stree)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar firstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(IS_HEAD_ROOTEDGE) {

        firstchar = *(stree->headedge.start);
        Uint rootchild = stree->rootchildren[(Uint) firstchar];

        if(IS_LEAF(rootchild)) {
            stree->split_vertex = rootchild;
            return;

        } else {

            vertex = stree->inner.first + INDEX(rootchild);
            update_chain(stree, vertex, &chainend, &distance);
            depth = get_depth(stree, vertex, distance, &chainend);

            Uint wlen = (stree->headedge.end - stree->headedge.start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                stree->split_vertex = rootchild;
                return;
            } else {
                // Go to successor vertex
                stree->headedge.vertex = vertex;
                stree->headedge.depth = depth;
            }

            // location has been scanned
            if(depth == wlen) {
                stree->headedge.end = NULL;
                return;
            } else {
                (stree->headedge.start) += depth;
            }
        }
    }

    while(True) {

        firstchar = *(stree->headedge.start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(stree->headedge.vertex);

        // traverse the list of successors
        while(True) {
            if(IS_LEAF(headchild)) {

                leafindex = INDEX(headchild);
                edgechar = text[stree->headedge.depth + leafindex];

                if(edgechar == firstchar) {
                    // correct edge found
                    stree->split_vertex = headchild;
                    stree->insertprev = prevnode;
                    return;
                }

                prevnode = headchild;
                headchild = stree->leaves.first[leafindex];
                continue;

            } else {

                // successor is branch node
                vertex = stree->inner.first + INDEX(headchild);
                update_chain(stree, vertex, &chainend, &distance);
                head = get_headpos(stree, vertex, &chainend, distance);
                edgechar = text[stree->headedge.depth + head];
                // Correct edge found
                if(edgechar == firstchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(vertex);
            }
        }

        depth = get_depth(stree, vertex, distance, &chainend);
        edgelen = depth - stree->headedge.depth;
        Uint wlen = (stree->headedge.end - stree->headedge.start + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            stree->split_vertex = headchild;
            stree->insertprev = prevnode;
            return;
        }
        // go to the successor node
        stree->headedge.vertex = vertex;
        stree->headedge.depth = depth;
        if(edgelen == wlen) {
            // location is found
            stree->headedge.end = NULL;
            return;
        }
        (stree->headedge.start) += edgelen;
    }
}

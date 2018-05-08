
#include "skip_count.h"

void skip_count(STree *stree)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar firstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(IS_HEADDEPTH_ZERO) {

        firstchar = *(stree->head_start);
        Uint rootchild = stree->rootchildren[(Uint) firstchar];

        if(IS_LEAF(rootchild)) {
            stree->split_vertex = rootchild;
            return;

        } else {

            vertex = stree->inner.first + LEAF_NUMBER(rootchild);
            update_chain(stree, vertex, &chainend, &distance);
            depth = get_depth(stree, vertex, distance, &chainend);

            Uint wlen = (stree->head_end - stree->head_start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                stree->split_vertex = rootchild;
                return;
            } else {
                // Go to successor vertex
                stree->headnode = vertex;
                stree->head_depth = depth;
            }

            // location has been scanned
            if(depth == wlen) {
                stree->head_end = NULL;
                return;
            } else {
                (stree->head_start) += depth;
            }
        }
    }

    while(True) {

        firstchar = *(stree->head_start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(stree->headnode);

        // traverse the list of successors
        while(True) {
            if(IS_LEAF(headchild)) {

                leafindex = LEAF_NUMBER(headchild);
                edgechar = text[stree->head_depth + leafindex];

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
                vertex = stree->inner.first + LEAF_NUMBER(headchild);
                update_chain(stree, vertex, &chainend, &distance);
                head = get_head(stree, vertex, &chainend, distance);
                edgechar = text[stree->head_depth + head];
                // Correct edge found
                if(edgechar == firstchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(vertex);
            }
        }

        depth = get_depth(stree, vertex, distance, &chainend);
        edgelen = depth - stree->head_depth;
        Uint wlen = (stree->head_end - stree->head_start + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            stree->split_vertex = headchild;
            stree->insertprev = prevnode;
            return;
        }
        // go to the successor node
        stree->headnode = vertex;
        stree->head_depth = depth;
        if(edgelen == wlen) {
            // location is found
            stree->head_end = NULL;
            return;
        }
        (stree->head_start) += edgelen;
    }
}


#include "skip_count.h"

void skip_count(STree *stree)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar firstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(IS_HEADDEPTH_ZERO) {

        firstchar = *(stree->headstart);
        Uint rootchild = stree->rootchildren[(Uint) firstchar];

        if(IS_LEAF(rootchild)) {
            stree->split_vertex = rootchild;
            return;

        } else {

            vertex = stree->inner.first + LEAF_NUM(rootchild);
            update_chain(stree, vertex, &chainend, &distance);
            depth = get_depth(stree, vertex, distance, &chainend);

            Uint wlen = (stree->vertex_succ_head - stree->headstart + 1);

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
                stree->vertex_succ_head = NULL;
                return;
            } else {
                (stree->headstart) += depth;
            }
        }
    }

    while(True) {

        firstchar = *(stree->headstart);
        prevnode = UNDEF;
        Vertex headchild = CHILD(stree->headnode);

        // traverse the list of successors
        while(True) {
            if(IS_LEAF(headchild)) {

                leafindex = LEAF_NUM(headchild);
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
                vertex = stree->inner.first + LEAF_NUM(headchild);
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
        Uint wlen = (stree->vertex_succ_head - stree->headstart + 1);
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
            stree->vertex_succ_head = NULL;
            return;
        }
        (stree->headstart) += edgelen;
    }
}

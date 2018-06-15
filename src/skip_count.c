
#include "skip_count.h"

void skip_count(STree *st)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar firstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(HEAD_IS_ROOT) {

        firstchar = *(st->head.label.start);
        Uint rootchild = st->rootchildren[(Uint) firstchar];

        if(IS_LEAF(rootchild)) {
            st->splitvertex.origin = rootchild;
            return;

        } else {

            vertex = INNER(rootchild);
            update_chain(st, vertex, &chainend, &distance);
            depth = get_depth(st, vertex, distance, &chainend);

            Uint wlen = (st->head.label.end - st->head.label.start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                st->splitvertex.origin = rootchild;
                return;
            } else {
                // Go to successor vertex
                st->head.origin = vertex;
                st->head.depth = depth;
            }

            // location has been scanned
            if(depth == wlen) {
                st->head.label.end = NULL;
                return;
            } else {
                (st->head.label.start) += depth;
            }
        }
    }

    while(True) {

        firstchar = *(st->head.label.start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(st->head.origin);

        // traverse the list of successors
        while(True) {
            if(IS_LEAF(headchild)) {

                leafindex = LEAF_INDEX(headchild);
                edgechar = text[st->head.depth + leafindex];

                if(edgechar == firstchar) {
                    // correct edge found
                    st->splitvertex.origin = headchild;
                    st->splitvertex.left_sibling = prevnode;
                    return;
                }

                prevnode = headchild;
                headchild = st->leaves.first[leafindex];
                continue;

            } else {

                // successor is branch node
                vertex = INNER(headchild);
                update_chain(st, vertex, &chainend, &distance);
                head = get_headpos(st, vertex, &chainend, distance);
                edgechar = text[st->head.depth + head];
                // Correct edge found
                if(edgechar == firstchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(vertex);
            }
        }

        depth = get_depth(st, vertex, distance, &chainend);
        edgelen = depth - st->head.depth;
        Uint wlen = (st->head.label.end - st->head.label.start + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            st->splitvertex.origin = headchild;
            st->splitvertex.left_sibling = prevnode;
            return;
        }
        // go to the successor node
        st->head.origin = vertex;
        st->head.depth = depth;
        if(edgelen == wlen) {
            // location is found
            st->head.label.end = NULL;
            return;
        }
        (st->head.label.start) += edgelen;
    }
}

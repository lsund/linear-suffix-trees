
#include "skip_count.h"

void skip_count(STree *st)
{
    Uint prevnode, depth, edgelen, leafindex, head;
    Wchar fstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(head_is_root(st)) {

        fstchar = *(st->head.label.start);
        Uint rootchild = st->rs[(Uint) fstchar];

        if(IS_LEAF(rootchild)) {
            st->split.child = rootchild;
            return;

        } else {

            vertex = VERTEX_TO_REF(rootchild);
            update_chain(st, vertex, &chainend, &distance);
            depth = get_depth(st, vertex, distance, &chainend);

            Uint wlen = (st->head.label.end - st->head.label.start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                st->split.child = rootchild;
                return;
            } else {
                // Go to successor vertex
                st->head.vertex = vertex;
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

    while(true) {

        fstchar = *(st->head.label.start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(st->head.vertex);

        // traverse the list of successors
        while(true) {
            if(IS_LEAF(headchild)) {

                leafindex = VERTEX_TO_INDEX(headchild);
                edgechar = text.content[st->head.depth + leafindex];

                if(edgechar == fstchar) {
                    // correct edge found
                    st->split.child = headchild;
                    st->split.left = prevnode;
                    return;
                }

                prevnode = headchild;
                headchild = st->ls.fst[leafindex];
                continue;

            } else {

                // successor is branch node
                vertex = VERTEX_TO_REF(headchild);
                update_chain(st, vertex, &chainend, &distance);
                head = get_headpos(st, vertex, &chainend, distance);
                edgechar = text.content[st->head.depth + head];
                // Correct edge found
                if(edgechar == fstchar) {
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
            st->split.child = headchild;
            st->split.left = prevnode;
            return;
        }
        // go to the successor node
        st->head.vertex = vertex;
        st->head.depth = depth;
        if(edgelen == wlen) {
            // location is found
            st->head.label.end = NULL;
            return;
        }
        (st->head.label.start) += edgelen;
    }
}

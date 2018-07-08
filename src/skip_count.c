
#include "skip_count.h"

void skip_count(STree *st)
{
    Uint prevnode, depth, edgelen, leafindex, hd;
    Wchar fstchar, edgechar;

    VertexP vertex  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(head_is_root(st)) {

        fstchar = *(st->hd.label.start);
        Uint rootchild = st->rs[(Uint) fstchar];

        if(IS_LEAF(rootchild)) {
            st->split.child = rootchild;
            return;

        } else {

            vertex = VERTEX_TO_REF(rootchild);
            set_dist_and_chainend(st, vertex, &chainend, &distance);
            depth = get_depth(st, vertex, distance, &chainend);

            Uint wlen = (st->hd.label.end - st->hd.label.start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                st->split.child = rootchild;
                return;
            } else {
                // Go to successor vertex
                st->hd.vertex = vertex;
                st->hd.depth = depth;
            }

            // location has been scanned
            if(depth == wlen) {
                st->hd.label.end = NULL;
                return;
            } else {
                (st->hd.label.start) += depth;
            }
        }
    }

    while(true) {

        fstchar = *(st->hd.label.start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(st->hd.vertex);

        // traverse the list of successors
        while(true) {
            if(IS_LEAF(headchild)) {

                leafindex = VERTEX_TO_INDEX(headchild);
                edgechar = text.fst[st->hd.depth + leafindex];

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
                set_dist_and_chainend(st, vertex, &chainend, &distance);
                hd = get_headpos(st, vertex, &chainend, distance);
                edgechar = text.fst[st->hd.depth + hd];
                // Correct edge found
                if(edgechar == fstchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(vertex);
            }
        }

        depth = get_depth(st, vertex, distance, &chainend);
        edgelen = depth - st->hd.depth;
        Uint wlen = (st->hd.label.end - st->hd.label.start + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            st->split.child = headchild;
            st->split.left = prevnode;
            return;
        }
        // go to the successor node
        st->hd.vertex = vertex;
        st->hd.depth = depth;
        if(edgelen == wlen) {
            // location is found
            st->hd.label.end = NULL;
            return;
        }
        (st->hd.label.start) += edgelen;
    }
}

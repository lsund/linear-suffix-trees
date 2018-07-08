
#include "skip_count.h"

void skip_count(STree *st)
{
    Uint prevnode, depth, edgelen, leafindex, hd;
    Wchar fstchar, edgechar;

    VertexP v  = NULL;;
    VertexP chainend = NULL;
    Uint distance = 0;

    if(head_is_root(st)) {

        fstchar = *(st->hd.l.start);
        Uint rootchild = st->rs[(Uint) fstchar];

        if(IS_LEAF(rootchild)) {
            st->split.child = rootchild;
            return;

        } else {

            v = VERTEX_TO_REF(rootchild);
            set_dist_and_chainend(st, v, &chainend, &distance);
            depth = get_depth(st, v, distance, &chainend);

            Uint wlen = (st->hd.l.end - st->hd.l.start + 1);

            if(depth > wlen) {
                // cannot reach the successor node
                st->split.child = rootchild;
                return;
            } else {
                // Go to successor v
                st->hd.v = v;
                st->hd.d = depth;
            }

            // location has been scanned
            if(depth == wlen) {
                st->hd.l.end = NULL;
                return;
            } else {
                (st->hd.l.start) += depth;
            }
        }
    }

    while(true) {

        fstchar = *(st->hd.l.start);
        prevnode = UNDEF;
        Vertex headchild = CHILD(st->hd.v);

        // traverse the list of successors
        while(true) {
            if(IS_LEAF(headchild)) {

                leafindex = VERTEX_TO_INDEX(headchild);
                edgechar = text.fst[st->hd.d + leafindex];

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
                v = VERTEX_TO_REF(headchild);
                set_dist_and_chainend(st, v, &chainend, &distance);
                hd = get_headpos(st, v, &chainend, distance);
                edgechar = text.fst[st->hd.d + hd];
                // Correct edge found
                if(edgechar == fstchar) {
                    break;
                }
                prevnode = headchild;
                headchild = SIBLING(v);
            }
        }

        depth = get_depth(st, v, distance, &chainend);
        edgelen = depth - st->hd.d;
        Uint wlen = (st->hd.l.end - st->hd.l.start + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            st->split.child = headchild;
            st->split.left = prevnode;
            return;
        }
        // go to the successor node
        st->hd.v = v;
        st->hd.d = depth;
        if(edgelen == wlen) {
            // location is found
            st->hd.l.end = NULL;
            return;
        }
        (st->hd.l.start) += edgelen;
    }
}

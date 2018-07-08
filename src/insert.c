#include "insert.h"


static void insert_rootleaf(STree *st, Wchar *start, Vertex v)
{
    st->rs[(Uint) *start] = v;
    LEAF_SIBLING(st->ls.nxt) = UNDEF;
}


// If there is only one other leaf where the new one is to be inserted, then
// simply make this its sibling.
static void insert_second_leaf(STree *st, Vertex v)
{
    LEAF_SIBLING(st->ls.nxt) = CHILD(st->hd.v);
    SET_CHILD(st->hd.v, v);
}


// If there is more than one leaf, then we have to check if the left sibling is
// an inner or leaf vertex.
static void insert_leaf(STree *st, Vertex v)
{
    if (IS_LEAF(st->split.left)) {

            Uint *prev               = VERTEX_TO_LEAFREF(st->split.left);
            LEAF_SIBLING(st->ls.nxt) = LEAF_SIBLING(prev);
            LEAF_SIBLING(prev)       = v;

    } else {

            Uint *prev               = VERTEX_TO_REF(st->split.left);
            LEAF_SIBLING(st->ls.nxt) = SIBLING(prev);
            SIBLING(prev)            = v;
    }
}


static void insert_new_inner(STree *st)
{
    if(head_is_root(st)) {

        st->rs[*st->hd.l.start] = st->is.nxt_ind;
        SIBLING(st->is.nxt) = UNDEF;

    } else if (!EXISTS(st->split.left)) {

        // If the split-edge has no left child, then the new inner vertex will
        // become the first child

        SET_CHILD(st->hd.v, st->is.nxt_ind);

    } else {

        // If split has a left sibling, then the new inner vertex will
        // become the child to the right of this.
        if(IS_LEAF(st->split.left)) {
            Uint *leftp = VERTEX_TO_LEAFREF(st->split.left);
            LEAF_SIBLING(leftp) = st->is.nxt_ind;

        } else {
            SIBLING(VERTEX_TO_REF(st->split.left)) = st->is.nxt_ind;
        }
    }
}


static void insert_leaf_under_split(STree *st)
{
    Wchar head_continuation = *(st->hd.l.end + 1);
    Wchar tail_continuation = *st->tail;

    // Then insert leafedge
    if(IS_LEAF(st->split.child)) {

        // split edge is leaf edge

        VertexP p_splitchild = VERTEX_TO_LEAFREF(st->split.child);

        // If the character of head is less than the one on the current tail,
        // then the new leaf edge is to be inserted 'to the right'.
        if (tail_at_lastchar(st) || head_continuation < tail_continuation) {

            // Splitchild becomes the first child
            SET_CHILD(st->is.nxt, st->split.child);
            // The inner vertex gets splitchilds sibling
            SIBLING(st->is.nxt) = LEAF_SIBLING(p_splitchild);
            // A new leaf edge is created from the newly inserted vertex
            LEAF_SIBLING(p_splitchild) = MAKE_LEAF(st->ls.nxt_ind);
            LEAF_SIBLING(st->ls.nxt)   = NOTHING;

        // Otherwise the new leaf is to be inserted 'to the left'
        } else {

            // The new leaf edge becomes the first child
            SET_CHILD(st->is.nxt, MAKE_LEAF(st->ls.nxt_ind));
            // Thi inner vertex gets splitchilds sibling
            SIBLING(st->is.nxt) = LEAF_SIBLING(p_splitchild);
            // The new vertex gets splitchild as sibling
            LEAF_SIBLING(st->ls.nxt) = st->split.child;
            LEAF_SIBLING(p_splitchild) = NOTHING;
        }

    } else {

        // splitchild is an inner vertex
        VertexP p_splitchild = VERTEX_TO_REF(st->split.child);

        // If the character of head is less than the one on the current tail,
        // then the new leaf edge is to be inserted 'to the right'.
        if (tail_at_lastchar(st) || head_continuation < tail_continuation) {

            // Splitchild becomes the first child
            SET_CHILD(st->is.nxt, st->split.child);
            // The inner vertex gets splitchilds sibling
            SIBLING(st->is.nxt) = SIBLING(p_splitchild);
            // new leaf = brother of old branch
            SIBLING(p_splitchild) = MAKE_LEAF(st->ls.nxt_ind);
            LEAF_SIBLING(st->ls.nxt) = NOTHING;

        } else {

            // First child is new leaf
            // Inherit brother
            SET_CHILD(st->is.nxt, MAKE_LEAF(st->ls.nxt_ind));
            SIBLING(st->is.nxt) = SIBLING(p_splitchild);
            // new branch is brother of new leaf
            LEAF_SIBLING(st->ls.nxt) = st->split.child;
            SIBLING(p_splitchild) = NOTHING;
        }
    }
}


static void update_labels(STree *st)
{
    Uint headlabel_length = (st->hd.l.end - st->hd.l.start + 1);
    st->c_depth           = st->hd.d + headlabel_length;
    DEPTH(st->is.nxt)     = st->c_depth;
    HEADPOS(st->is.nxt)   = st->ls.nxt_ind;
}



// head is already a vertex. A new leaf edge is supposed to be inserted under
// it. In this case, split == head
void insert(STree *st)
{
    Uint leaf = MAKE_LEAF(st->ls.nxt_ind);

    if (head_is_root(st)) {

        if (!tail_at_lastchar(st)) {
            insert_rootleaf(st, st->tail, leaf);
        }

    } else {

        // head is the only child
        if (!EXISTS(st->split.left)) {
            insert_second_leaf(st, leaf);
        } else {
            insert_leaf(st, leaf);
        }
    }

    st->ls.nxt_ind++;
    st->ls.nxt++;
}


void split_and_insert(STree *st)
{
    allocate_inner_vertices(st);

    // First set the new inner vertex as child to the vertex where the
    // splitedge was hanging from.
    insert_new_inner(st);
    // Then insert a new leaf under that inner vertex and attach the splitedge.
    insert_leaf_under_split(st);
    // Finally update the labels
    update_labels(st);

    // Since a new leaf was created, increment leaves
    st->ls.nxt_ind++;
    st->ls.nxt += LEAF_VERTEXSIZE;
}


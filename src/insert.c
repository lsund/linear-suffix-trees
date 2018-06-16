#include "insert.h"

Wchar *sentinel;

static void insert_tail_under_root(STree *st, Vertex v)
{
    st->rootchildren[(Uint) *st->tail] = v;
    *st->ls.nxt = 0;
}


static void insert_first_leaf(STree *st, Vertex v)
{
    set_next_leaf(st, CHILD(st->head.vertex));
    SET_CHILD(st->head.vertex, v);
}


static void insert_successor_leaf(STree *st, Vertex v)
{
    if (IS_LEAF(st->splitchild.left)) {

            Uint *prev = VERTEX_TO_LEAFREF(st->splitchild.left);
            *st->ls.nxt = LEAF_SIBLING(prev);
            LEAF_SIBLING(prev) = v;

    } else {

            Uint *prev = VERTEX_TO_INNERREF(st->splitchild.left);
            *st->ls.nxt = SIBLING(prev);
            SIBLING(prev) = v;
    }
}

// head is already a vertex. A new leaf edge is supposed to be inserted under
// it. In this case, splitchild == head
void insert(STree *st)
{
    Uint leaf = get_next_leafnum(st);

    if (head_is_root(st)) {

        if (!tail_at_lastchar(st)) {
            insert_tail_under_root(st, leaf);
        }

    } else {

        // head is the only child
        if (!EXISTS(st->splitchild.left)) {

            insert_first_leaf(st, leaf);
        } else {
            insert_successor_leaf(st, leaf);
        }
    }

    st->ls.nxt_ind++;
    st->ls.nxt++;
}

void split_and_insert(STree *st)
{
    Uint *inserted, insert_sibling;

    allocate_inner_vertices(st);

    if(head_is_root(st)) {

        SET_ROOTCHILD(*st->head.label.start, st->is.nxt_ind);
        *(st->is.nxt + 1) = 0;

    } else if (IS_LEFTMOST(st->splitchild.left)) {
        // new branch = fst child
        SET_CHILD(st->head.vertex, st->is.nxt_ind);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(st->splitchild.left)) {
            Uint *ptr = VERTEX_TO_LEAFREF(st->splitchild.left);
            LEAF_SIBLING(ptr) = st->is.nxt_ind;
        } else {
            SIBLING(VERTEX_TO_INNERREF(st->splitchild.left)) = st->is.nxt_ind;
        }
    }
    if(IS_LEAF(st->splitchild.vertex)) {
        // split edge is leaf edge
        inserted = VERTEX_TO_LEAFREF(st->splitchild.vertex);
        if (tail_at_lastchar(st) || *(st->head.label.end + 1) < *(st->tail))
        {
            // fst child =oldleaf
            // inherit brother
            SET_CHILD(st->is.nxt, st->splitchild.vertex);
            SIBLING(st->is.nxt) = *inserted;
            // Recall new leaf address
            *st->ls.nxt = NOTHING;
            LEAF_SIBLING(inserted) = get_next_leafnum(st);
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD(st->is.nxt, get_next_leafnum(st));
            SIBLING(st->is.nxt) = *inserted;
            *(st->ls.nxt) = st->splitchild.vertex;  // old leaf = right brother of of new leaf
            // Recall leaf address
            *inserted = NOTHING;
        }
    } else {
        // split edge leads to branching node
        inserted = VERTEX_TO_INNERREF(st->splitchild.vertex);
        insert_sibling = SIBLING(inserted);
        if (st->tail == sentinel ||
                *(st->head.label.end+1) < *(st->tail))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD(st->is.nxt, st->splitchild.vertex);
            SIBLING(st->is.nxt) = insert_sibling;
            // Recall new leaf address
            *st->ls.nxt = NOTHING;
            // new leaf = brother of old branch
            SIBLING(inserted) = MAKE_LEAF(st->ls.nxt_ind);
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD(st->is.nxt, MAKE_LEAF(st->ls.nxt_ind));
            SIBLING(st->is.nxt) = insert_sibling;
            *(st->ls.nxt) = st->splitchild.vertex;   // new branch is brother of new leaf
            *(inserted + 1) = NOTHING;
        }
    }
    Uint headlabel_length      = (st->head.label.end - st->head.label.start + 1);
    st->current_branchdepth = st->head.depth + headlabel_length;
    DEPTH(st->is.nxt)   = st->current_branchdepth;
    HEADPOS(st->is.nxt) = st->ls.nxt_ind;
    st->ls.nxt_ind++;
    st->ls.nxt++;
}


#include "insert.h"

Wchar *sentinel;

void insert(STree *st)
{
    Uint leaf = get_next_leafnum(st);

    if (HEAD_IS_ROOT) {

        if (!IS_SENTINEL(st->tail)) {
            SET_ROOTCHILD(*(st->tail), leaf);
            *st->leaves.next = 0;
        }

    } else {

        if (IS_UNDEF(st->splitvertex.left_sibling)) {

            set_next_leaf(st, CHILD(st->head.origin));

            SET_CHILD(st->head.origin, leaf);

        } else {

            if (IS_LEAF(st->splitvertex.left_sibling)) {

                Uint *prev = LEAF(st->splitvertex.left_sibling);
                *st->leaves.next = LEAF_SIBLING(prev);
                LEAF_SIBLING(prev) = leaf;

            } else {

                Uint *prev = INNER(st->splitvertex.left_sibling);
                *st->leaves.next = SIBLING(prev);
                SIBLING(prev) = leaf;
            }
        }
    }

    st->leaves.next_ind++;
    st->leaves.next++;
}

void split_and_insert(STree *st)
{
    Uint *inserted, insert_sibling;

    allocate_inner_vertices(st);

    if(HEAD_IS_ROOT) {

        SET_ROOTCHILD(*st->head.label.start, st->inner.next_ind);
        *(st->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(st->splitvertex.left_sibling)) {
        // new branch = first child
        SET_CHILD(st->head.origin, st->inner.next_ind);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(st->splitvertex.left_sibling)) {
            Uint *ptr = LEAF(st->splitvertex.left_sibling);
            LEAF_SIBLING(ptr) = st->inner.next_ind;
        } else {
            SIBLING(INNER(st->splitvertex.left_sibling)) = st->inner.next_ind;
        }
    }
    if(IS_LEAF(st->splitvertex.origin)) {
        // split edge is leaf edge
        inserted = LEAF(st->splitvertex.origin);
        if (IS_SENTINEL(st->tail) ||
                *(st->head.label.end + 1) < *(st->tail))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD(st->inner.next, st->splitvertex.origin);
            SIBLING(st->inner.next) = *inserted;
            // Recall new leaf address
            *st->leaves.next = NOTHING;
            LEAF_SIBLING(inserted) = get_next_leafnum(st);
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD(st->inner.next, get_next_leafnum(st));
            SIBLING(st->inner.next) = *inserted;
            *(st->leaves.next) = st->splitvertex.origin;  // old leaf = right brother of of new leaf
            // Recall leaf address
            *inserted = NOTHING;
        }
    } else {
        // split edge leads to branching node
        inserted = INNER(st->splitvertex.origin);
        insert_sibling = SIBLING(inserted);
        if (st->tail == sentinel ||
                *(st->head.label.end+1) < *(st->tail))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD(st->inner.next, st->splitvertex.origin);
            SIBLING(st->inner.next) = insert_sibling;
            // Recall new leaf address
            *st->leaves.next = NOTHING;
            // new leaf = brother of old branch
            SIBLING(inserted) = MAKE_LEAF(st->leaves.next_ind);
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD(st->inner.next, MAKE_LEAF(st->leaves.next_ind));
            SIBLING(st->inner.next) = insert_sibling;
            *(st->leaves.next) = st->splitvertex.origin;   // new branch is brother of new leaf
            *(inserted + 1) = NOTHING;
        }
    }
    Uint headlabel_length      = (st->head.label.end - st->head.label.start + 1);
    st->current_branchdepth = st->head.depth + headlabel_length;
    DEPTH(st->inner.next)   = st->current_branchdepth;
    HEADPOS(st->inner.next) = st->leaves.next_ind;
    st->leaves.next_ind++;
    st->leaves.next++;
}


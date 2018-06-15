#include "insert.h"

Wchar *sentinel;

static Uint *new_allocbound(STree *stree)
{
    return stree->inner.first + stree->inner.size - LARGE_VERTEXSIZE;
}

// Allocate space for branch vertices
static void allocate_inner_vertices(STree *stree)
{
    if(IS_NO_SPACE) {

        stree->inner.size += EXTRA_ALLOCSIZE;

        Uint chain;
        if(stree->chain.first != NULL) {
            chain = REF_TO_INDEX(stree->chain.first);
        }

        Uint head = REF_TO_INDEX(stree->head.origin);
        Uint size = stree->inner.size;

        stree->inner.first = realloc(stree->inner.first, sizeof(Uint) * size);
        stree->inner.next  = next_inner(stree);
        stree->head.origin = stree->inner.first + head;

        if(stree->chain.first != NULL) {
            stree->chain.first = stree->inner.first + chain;
        }
        stree->allocated = new_allocbound(stree);
    }
}

void insert(STree *stree)
{
    Uint leaf = MAKE_LEAF(stree->leaves.next_ind);

    if (HEAD_IS_ROOT) {

        if (!IS_SENTINEL(stree->tail)) {
            SET_ROOTCHILD(*(stree->tail), leaf);
            *stree->leaves.next = 0;
        }

    } else {

        if (IS_UNDEF(stree->splitvertex.left_sibling)) {

            *stree->leaves.next = CHILD(stree->head.origin);
            SET_CHILD(stree->head.origin, leaf);

        } else {

            if (IS_LEAF(stree->splitvertex.left_sibling)) {

                Uint *prev = LEAF(stree->splitvertex.left_sibling);
                *stree->leaves.next = LEAF_SIBLING(prev);
                LEAF_SIBLING(prev) = leaf;

            } else {

                Uint *prev = INNER(stree->splitvertex.left_sibling);
                *stree->leaves.next = SIBLING(prev);
                SIBLING(prev) = leaf;
            }
        }
    }

    stree->leaves.next_ind++;
    stree->leaves.next++;
}

void split_and_insert(STree *stree)
{
    Uint *inserted, insert_sibling;

    allocate_inner_vertices(stree);

    if(HEAD_IS_ROOT) {

        SET_ROOTCHILD(*stree->head.label.start, stree->inner.next_ind);
        *(stree->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(stree->splitvertex.left_sibling)) {
        // new branch = first child
        SET_CHILD(stree->head.origin, stree->inner.next_ind);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(stree->splitvertex.left_sibling)) {
            Uint *ptr = LEAF(stree->splitvertex.left_sibling);
            LEAF_SIBLING(ptr) = stree->inner.next_ind;
        } else {
            SIBLING(INNER(stree->splitvertex.left_sibling)) = stree->inner.next_ind;
        }
    }
    if(IS_LEAF(stree->splitvertex.origin)) {
        // split edge is leaf edge
        inserted = LEAF(stree->splitvertex.origin);
        if (IS_SENTINEL(stree->tail) ||
                *(stree->head.label.end + 1) < *(stree->tail))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD(stree->inner.next, stree->splitvertex.origin);
            SIBLING(stree->inner.next) = *inserted;
            // Recall new leaf address
            *stree->leaves.next = NOTHING;
            LEAF_SIBLING(inserted) = MAKE_LEAF(stree->leaves.next_ind);
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD(stree->inner.next, MAKE_LEAF(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = *inserted;
            *(stree->leaves.next) = stree->splitvertex.origin;  // old leaf = right brother of of new leaf
            // Recall leaf address
            *inserted = NOTHING;
        }
    } else {
        // split edge leads to branching node
        inserted = INNER(stree->splitvertex.origin);
        insert_sibling = SIBLING(inserted);
        if (stree->tail == sentinel ||
                *(stree->head.label.end+1) < *(stree->tail))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD(stree->inner.next, stree->splitvertex.origin);
            SIBLING(stree->inner.next) = insert_sibling;
            // Recall new leaf address
            *stree->leaves.next = NOTHING;
            // new leaf = brother of old branch
            SIBLING(inserted) = MAKE_LEAF(stree->leaves.next_ind);
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD(stree->inner.next, MAKE_LEAF(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = insert_sibling;
            *(stree->leaves.next) = stree->splitvertex.origin;   // new branch is brother of new leaf
            *(inserted + 1) = NOTHING;
        }
    }
    Uint headlabel_length      = (stree->head.label.end - stree->head.label.start + 1);
    stree->current_branchdepth = stree->head.depth + headlabel_length;
    DEPTH(stree->inner.next)   = stree->current_branchdepth;
    HEADPOS(stree->inner.next) = stree->leaves.next_ind;
    stree->leaves.next_ind++;
    stree->leaves.next++;
}


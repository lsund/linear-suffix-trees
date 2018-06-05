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

        stree->inner.first = ALLOC(stree->inner.first, Uint, size);
        stree->inner.next = stree->inner.first + stree->inner.next_ind;
        stree->head.origin = stree->inner.first + head;

        if(stree->chain.first != NULL) {
            stree->chain.first = stree->inner.first + chain;
        }
        stree->allocated = new_allocbound(stree);
    }
}

void insert_leaf(STree *stree)
{
    Uint leaf = WITH_LEAFBIT(stree->leaves.next_ind);

    if (head_depth(stree) == 0) {
        if (!IS_SENTINEL(stree->tail)) {
            SET_ROOTCHILD(*(stree->tail), leaf);
            *stree->leaves.next = 0;
        }
    }

    else {
        if (IS_LEFTMOST(stree->insertprev)) {

            *stree->leaves.next = CHILD(stree->head.origin);
            SET_CHILD(stree->head.origin, leaf);

        } else {
            if (IS_LEAF(stree->insertprev)) {

                // Previous node is leaf
                Uint *prev = stree->leaves.first + INDEX(stree->insertprev);
                *stree->leaves.next = LEAF_SIBLING(prev);

                LEAF_SIBLING(prev) = leaf;

            } else {
                // previous node is branching node
                Uint *prev = stree->inner.first + INDEX(stree->insertprev);
                *stree->leaves.next = SIBLING(prev);
                SIBLING(prev) = leaf;
            }
        }
    }

    stree->leaves.next_ind++;
    stree->leaves.next++;
}

void insert_inner(STree *stree)
{
    Uint *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(head_depth(stree) == 0) {

        SET_ROOTCHILD(*stree->head.label.start, stree->inner.next_ind);
        *(stree->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {
        // new branch = first child
        SET_CHILD(stree->head.origin, stree->inner.next_ind);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(stree->insertprev)) {
            Uint *ptr = stree->leaves.first + INDEX(stree->insertprev);
            LEAF_SIBLING(ptr) = stree->inner.next_ind;
        } else {
            SIBLING(INNER(stree->insertprev)) = stree->inner.next_ind;
        }
    }
    if(IS_LEAF(stree->split_vertex)) {
        // split edge is leaf edge
        insertleafptr = stree->leaves.first + INDEX(stree->split_vertex);
        if (stree->tail == sentinel ||
                *(stree->head.label.end + 1) < *(stree->tail))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD(stree->inner.next, stree->split_vertex);
            SIBLING(stree->inner.next) = *insertleafptr;
            // Recall new leaf address
            *stree->leaves.next = NOTHING;
            LEAF_SIBLING(insertleafptr) = WITH_LEAFBIT(stree->leaves.next_ind);
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD(stree->inner.next, WITH_LEAFBIT(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = *insertleafptr;
            *(stree->leaves.next) = stree->split_vertex;  // old leaf = right brother of of new leaf
            // Recall leaf address
            *insertleafptr = NOTHING;
        }
    } else {
        // split edge leads to branching node
        insertnodeptr = stree->inner.first + INDEX(stree->split_vertex);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tail == sentinel ||
                *(stree->head.label.end+1) < *(stree->tail))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD(stree->inner.next, stree->split_vertex);
            SIBLING(stree->inner.next) = insertnodeptrbrother;
            // Recall new leaf address
            *stree->leaves.next = NOTHING;
            // new leaf = brother of old branch
            SIBLING(insertnodeptr) = WITH_LEAFBIT(stree->leaves.next_ind);
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD(stree->inner.next, WITH_LEAFBIT(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = insertnodeptrbrother;
            *(stree->leaves.next) = stree->split_vertex;   // new branch is brother of new leaf
            *(insertnodeptr + 1) = NOTHING;
        }
    }
    stree->currentdepth = stree->head.depth + (Uint) (stree->head.label.end - stree->head.label.start+1);
    DEPTH(stree->inner.next) = stree->currentdepth;
    HEADPOS(stree->inner.next) =stree->leaves.next_ind;
    stree->leaves.next_ind++;
    stree->leaves.next++;
}


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

        Uint head = REF_TO_INDEX(stree->headedge.origin);
        Uint size = stree->inner.size;

        stree->inner.first = ALLOC(stree->inner.first, Uint, size);
        stree->inner.next = stree->inner.first + stree->inner.next_ind;
        stree->headedge.origin = stree->inner.first + head;

        if(stree->chain.first != NULL) {
            stree->chain.first = stree->inner.first + chain;
        }
        stree->allocated = new_allocbound(stree);
    }
}

void insert_leaf(STree *stree)
{
    Uint leaf = WITH_LEAFBIT(stree->leaves.next_ind);

    if(IS_DEPTH_0 && !IS_SENTINEL(stree->tailptr)) {

        SET_ROOTCHILD(*(stree->tailptr), leaf);
        *stree->leaves.next = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {

        *stree->leaves.next = CHILD(stree->headedge.origin);
        SET_CHILD(stree->headedge.origin, leaf);

    } else if (IS_LEAF(stree->insertprev)) {

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

    stree->leaves.next_ind++;
    stree->leaves.next++;
}

void insert_inner(STree *stree)
{
    Uint *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(IS_DEPTH_0) {

        SET_ROOTCHILD(*(stree->headedge.start), stree->inner.next_ind);
        *(stree->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {
        // new branch = first child
        SET_CHILD(stree->headedge.origin, stree->inner.next_ind);
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
        if (stree->tailptr == sentinel ||
                *(stree->headedge.end + 1) < *(stree->tailptr))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD(stree->inner.next, stree->split_vertex);
            SIBLING(stree->inner.next) = *insertleafptr;
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->is_nil_stored = True;
            LEAF_SIBLING(insertleafptr) = WITH_LEAFBIT(stree->leaves.next_ind);
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD(stree->inner.next, WITH_LEAFBIT(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = *insertleafptr;
            *(stree->leaves.next) = stree->split_vertex;  // old leaf = right brother of of new leaf
            // Recall leaf address
            stree->setlink = insertleafptr;
            stree->is_nil_stored = False;
        }
    } else {
        // split edge leads to branching node
        insertnodeptr = stree->inner.first + INDEX(stree->split_vertex);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tailptr == sentinel ||
                *(stree->headedge.end+1) < *(stree->tailptr))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD(stree->inner.next, stree->split_vertex);
            SIBLING(stree->inner.next) = insertnodeptrbrother;
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->is_nil_stored = True;
            // new leaf = brother of old branch
            SIBLING(insertnodeptr) = WITH_LEAFBIT(stree->leaves.next_ind);
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD(stree->inner.next, WITH_LEAFBIT(stree->leaves.next_ind));
            SIBLING(stree->inner.next) = insertnodeptrbrother;
            *(stree->leaves.next) = stree->split_vertex;   // new branch is brother of new leaf
            stree->setlink = insertnodeptr + 1;
            stree->is_nil_stored = False;
        }
    }
    *(stree->setlink) = NOTHING;
    stree->currentdepth = stree->headedge.depth + (Uint) (stree->headedge.end - stree->headedge.start+1);
    DEPTH(stree->inner.next) = stree->currentdepth;
    HEADPOS(stree->inner.next) =stree->leaves.next_ind;
    if (stree->currentdepth > stree->maxbranchdepth) {
        stree->maxbranchdepth = stree->currentdepth;
    }
    stree->leaves.next_ind++;
    stree->leaves.next++;
}


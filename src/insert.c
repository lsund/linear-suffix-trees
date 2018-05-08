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
            chain = INDEX(stree->chain.first);
        }

        Uint head = INDEX(stree->headedge.vertex);
        Uint size = stree->inner.size;

        stree->inner.first = ALLOC(stree->inner.first, Uint, size);
        stree->inner.next = stree->inner.first + stree->inner.next_num;
        stree->headedge.vertex = stree->inner.first + head;

        if(stree->chain.first != NULL) {
            stree->chain.first = stree->inner.first + chain;
        }
        stree->allocated = new_allocbound(stree);
    }
}

void insert_leaf(STree *stree)
{
    Uint leaf = MAKE_LEAF(stree->leaves.next_num);

    if(IS_HEAD_ROOTEDGE && !IS_SENTINEL(stree->tailptr)) {

        SET_ROOTCHILD(*(stree->tailptr), leaf);
        *stree->leaves.next = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {

        *stree->leaves.next = CHILD(stree->headedge.vertex);
        SET_CHILD(stree->headedge.vertex, leaf);

    } else if (IS_LEAF(stree->insertprev)) {

        // Previous node is leaf
        Uint *prev = stree->leaves.first + LEAF_NUMBER(stree->insertprev);
        *stree->leaves.next = LEAF_SIBLING(prev);
        SET_LEAF_SIBLING(prev, leaf);

    } else {
        // previous node is branching node
        Uint *prev = stree->inner.first + LEAF_NUMBER(stree->insertprev);
        *stree->leaves.next = SIBLING(prev);
        SET_SIBLING(prev, leaf);
    }

    stree->leaves.next_num++;
    stree->leaves.next++;
}

void insert_inner(STree *stree)
{
    Uint *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(IS_HEAD_ROOTEDGE) {

        SET_ROOTCHILD(*(stree->headedge.start), stree->inner.next_num);
        *(stree->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {
        // new branch = first child
        SET_CHILD(stree->headedge.vertex,stree->inner.next_num);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(stree->insertprev)) {
            Uint *ptr = stree->leaves.first + LEAF_NUMBER(stree->insertprev);
            SET_LEAF_SIBLING(ptr,stree->inner.next_num);
        } else {
            // new branch = brother of branching node
            SET_SIBLING(stree->inner.first + LEAF_NUMBER(stree->insertprev),
                    stree->inner.next_num);
        }
    }
    if(IS_LEAF(stree->split_vertex)) {
        // split edge is leaf edge
        insertleafptr = stree->leaves.first + LEAF_NUMBER(stree->split_vertex);
        if (stree->tailptr == sentinel ||
                *(stree->headedge.end + 1) < *(stree->tailptr))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->split_vertex), *insertleafptr);
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->is_nil_stored = True;
            SET_LEAF_SIBLING(insertleafptr,                     // new leaf =
                    MAKE_LEAF(stree->leaves.next_num)); // right brother of old leaf
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(stree->leaves.next_num), *insertleafptr);
            *(stree->leaves.next) = stree->split_vertex;  // old leaf = right brother of of new leaf
            // Recall leaf address
            stree->setlink = insertleafptr;
            stree->is_nil_stored = False;
        }
    } else {
        // split edge leads to branching node
        insertnodeptr = stree->inner.first + LEAF_NUMBER(stree->split_vertex);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tailptr == sentinel ||
                *(stree->headedge.end+1) < *(stree->tailptr))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->split_vertex), insertnodeptrbrother);
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->is_nil_stored = True;
            SET_SIBLING(insertnodeptr,MAKE_LEAF(stree->leaves.next_num)); // new leaf = brother of old branch
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(stree->leaves.next_num), insertnodeptrbrother);
            *(stree->leaves.next) = stree->split_vertex;   // new branch is brother of new leaf
            stree->setlink = insertnodeptr + 1;
            stree->is_nil_stored = False;
        }
    }
    *(stree->setlink) = NOTHING;
    stree->currentdepth = stree->headedge.depth + (Uint) (stree->headedge.end - stree->headedge.start+1);
    SET_DEPTH(stree->currentdepth);
    SET_HEAD(stree->leaves.next_num);
    if (stree->currentdepth > stree->maxbranchdepth) {
        stree->maxbranchdepth = stree->currentdepth;
    }
    stree->leaves.next_num++;
    stree->leaves.next++;
}


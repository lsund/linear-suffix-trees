#include "insert.h"

Wchar *sentinel;

static Uint *new_allocbound(STree *stree)
{
    return stree->inner.first + stree->inner.size - LARGE_WIDTH;
}

// Allocate space for branch vertices
static void allocate_inner_vertices(STree *stree)
{
    if(IS_NO_SPACE) {

        stree->inner.size += EXTRA_ALLOCSIZE;

        Uint chainstart;
        if(stree->chainstart != NULL) {
            chainstart = INDEX(stree->chainstart);
        }

        Uint head = INDEX(stree->headnode);
        Uint size = stree->inner.size;

        stree->inner.first = ALLOC(stree->inner.first, Uint, size);
        stree->inner.next = stree->inner.first + stree->inner.next_num;
        stree->headnode = stree->inner.first + head;

        if(stree->chainstart != NULL) {
            stree->chainstart = stree->inner.first + chainstart;
        }
        stree->allocated = new_allocbound(stree);
    }
}

void insert_leaf(STree *stree)
{
    Uint leaf = MAKE_LEAF(stree->leaves.next_num);

    if(IS_HEAD_ROOT && !IS_SENTINEL(stree->tailptr)) {

        SET_ROOTCHILD(*(stree->tailptr), leaf);
        *stree->leaves.next = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {

        *(stree->leaves.next) = CHILD(stree->headnode);
        SET_CHILD(stree->headnode, leaf);

    } else if (IS_LEAF(stree->insertprev)) {

        // Previous node is leaf
        Uint *prev = stree->leaves.first + LEAF_NUM(stree->insertprev);
        *(stree->leaves.next) = LEAF_SIBLING(prev);
        SET_LEAF_SIBLING(prev, leaf);

    } else {
        // previous node is branching node
        Uint *prev = stree->inner.first + LEAF_NUM(stree->insertprev);
        *(stree->leaves.next) = SIBLING(prev);
        SET_SIBLING(prev, leaf);
    }

    stree->leaves.next_num++;
    stree->leaves.next++;
}

void insert_inner(STree *stree)
{
    Uint *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(IS_ROOT_DEPTH) {

        SET_ROOTCHILD(*(stree->headstart), stree->inner.next_num);
        *(stree->inner.next + 1) = 0;

    } else if (IS_LEFTMOST(stree->insertprev)) {
        // new branch = first child
        SET_CHILD(stree->headnode,stree->inner.next_num);
    } else {
        // new branch = right brother of leaf
        if(IS_LEAF(stree->insertprev)) {
            Uint *ptr = stree->leaves.first + LEAF_NUM(stree->insertprev);
            SET_LEAF_SIBLING(ptr,stree->inner.next_num);
        } else {
            // new branch = brother of branching node
            SET_SIBLING(stree->inner.first + LEAF_NUM(stree->insertprev),
                    stree->inner.next_num);
        }
    }
    if(IS_LEAF(stree->split_vertex)) {
        // split edge is leaf edge
        insertleafptr = stree->leaves.first + LEAF_NUM(stree->split_vertex);
        if (stree->tailptr == sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->split_vertex), *insertleafptr);
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->setatnewleaf = True;
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
            stree->setatnewleaf = False;
        }
    } else {
        // split edge leads to branching node
        insertnodeptr = stree->inner.first + LEAF_NUM(stree->split_vertex);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tailptr == sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->split_vertex), insertnodeptrbrother);
            // Recall new leaf address
            stree->setlink = stree->leaves.next;
            stree->setatnewleaf = True;
            SET_SIBLING(insertnodeptr,MAKE_LEAF(stree->leaves.next_num)); // new leaf = brother of old branch
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(stree->leaves.next_num), insertnodeptrbrother);
            *(stree->leaves.next) = stree->split_vertex;   // new branch is brother of new leaf
            stree->setlink = insertnodeptr + 1;
            stree->setatnewleaf = False;
        }
    }
    *(stree->setlink) = NOTHING;
    stree->currentdepth = stree->head_depth + (Uint) (stree->headend - stree->headstart+1);
    SET_DEPTH(stree->currentdepth);
    SET_HEAD(stree->leaves.next_num);
    if (stree->currentdepth > stree->maxbranchdepth) {
        stree->maxbranchdepth = stree->currentdepth;
    }
    stree->leaves.next_num++;
    stree->leaves.next++;
}


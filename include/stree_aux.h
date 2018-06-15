#ifndef STREE_AUX_H
#define STREE_AUX_H

#include "streedata.h"
#include "bitvector.h"

#define LEAF_VERTEXSIZE             1
#define SMALL_VERTEXSIZE            3
#define LARGE_VERTEXSIZE            5

// The label for a incoming edge to a vertex wu can be obtained by dropping
// depth(w) characters of wu.
#define LABEL_START(O)          text + (O)

#define START_ALLOCSIZE         max(0.5 * SMALL_VERTEXSIZE * (textlen + 1), 48);
#define EXTRA_ALLOCSIZE         max(0.05 * SMALL_VERTEXSIZE * (textlen + 1), 48);


///////////////////////////////////////////////////////////////////////////////
// Bitdefs

#define LEAFBIT                   SECOND_MSB         // mark leaf address
#define SMALLBIT                  MSB               // marks a small node
#define NOTHING                   MSB               // Marks a nil referenc
#define MAXINDEX                  (MSB - 1)         // Second biggest value
#define MAXDISTANCE               MAXINDEX          // maximal distance value
#define MAXTEXTLEN                ((MAXINDEX / ((LARGE_VERTEXSIZE+SMALL_VERTEXSIZE) / 2)) - 3)
#define UNDEF                     (~((Uint) 0))    // All ones


///////////////////////////////////////////////////////////////////////////////
// Queries

// An extra bit
// with each such integer tells whereth the reference is to a leaf or to a
// branching vertex
#define IS_LEAF(V)                  ((V) & LEAFBIT)
#define IS_SMALL(V)                 ((V) & SMALLBIT)
#define IS_LARGE(V)                 (!((V) & SMALLBIT))
#define IS_LAST(C)                  ((C) >= sentinel)
#define IS_SENTINEL(C)              ((C) == sentinel)
#define IS_NOTHING(P)               ((P) & NOTHING)
#define IS_SOMETHING(P)             (!IS_NOTHING((P)))
#define IS_ROOT(V)                  (stree->inner.first == V)
#define IS_UNDEF(V)                 ((V) == UNDEF)
#define IS_LEFTMOST(V)              ((V) == UNDEF)

#define HEAD_IS_ROOT                (stree->head.depth == 0)

///////////////////////////////////////////////////////////////////////////////
// Vertices

#define ROOT                    (stree->inner.first)
#define ROOT_CHILD(C)           (stree->rootchildren[(Uint) (C)])
// Index of a reference
#define REF_TO_INDEX(P)         ((Uint) ((P) - ROOT))
#define LEAFREF_TO_INDEX(ST,A)  ((Uint) ((A) - (ST)->leaves.first))

// Returns the sibling of the leaf at the specified address
#define INNER(V)               stree->inner.first + LEAF_INDEX((V)) // address
#define LEAF(V)                stree->leaves.first + INNER_INDEX((V))
#define MAKE_LEAF(V)            ((V) | LEAFBIT)
#define MAKE_SMALL(V)           ((V) | SMALLBIT)

#define LEAF_SIBLING(P)        (*(P))

// LEAF
#define LEAF_INDEX(V)           ((V) & ~(MSB | SECOND_MSB))
#define INNER_INDEX(V)          ((V) & ~(MSB))

// INNER
#define CHILD(P)               ((*(P)) & ~(MSB))

#define SIBLING(P)             (*((P) + 1))

// Small inner
#define DISTANCE(P)            (*((P) + 2))
// Large inner
#define DEPTH(P)               (*((P) + 2))
#define HEADPOS(P)             (*((P) + 3))
#define SUFFIX_LINK(P)         (*((P) + 4))

#define CHAIN_END(P, D)        (P) + SMALL_VERTEXSIZE * (D)

///////////////////////////////////////////////////////////////////////////////
// Setters

#define SET_ROOTCHILD(I, C)             (stree->rootchildren[(Uint) (I)]) = (C)
#define SET_CHILD(V, VAL)               *(V) = ((*(V)) & MSB) | (VAL)

Bool base_is_vertex(STree *stree);

Bool is_head_old(STree *stree);

// Get info for branch vertex
Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

void update_chain(STree *stree, Uint *vertexp, Uint **largep, Uint *distance);

// The smallest integer i such that vertexp = head(i)
Uint get_headpos(STree *stree, Uint *vertexp, Uint **largep, Uint distance);

Uint get_depth(STree *stree, Uint *vertexp, Uint distance, Uint **largep);

void follow_link(STree *stree);

void set_child_and_sibling(STree *stree, Uint child, Uint sibling);

#endif

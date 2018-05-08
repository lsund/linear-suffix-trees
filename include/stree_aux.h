#ifndef STREE_AUX_H
#define STREE_AUX_H

#include "streedata.h"
#include "bitvector.h"

// A vertex aw is small if headposition(aw) + 1 = headposition(w)
// A vertex aw is large if headposition(aw) > headposition(w).
// The root is neither small or large.
//
// A small vertex is always directly followed by another branching vertex, and
// the last branching vertex is a large vertex
//
// We can thus partition a sequence of inner vertices into chains of zero or
// more consecutive small vertices followed by a large vertex. This is a
// contigous subsequence of a inner vertex sequence, such that the first is not
// small, the last is large and the rest are small. Thus every inner vertex
// (not the root) is a member of exactly one chain.
//
// This exploits some redundancies, which allows for the improved implementation.
#define SMALL_VERTEXSIZE           3                   // # of integers for small node
#define LARGE_VERTEXSIZE           5                   // # of integers for large node

/*
   We use the least significant bit to discriminate references to leafs
   and branching nodes. Since base addresses are even, the unset least
   significant bit of a reference identifies a base address. For a leaf
   reference we shift the leaf number one to the right and
   set the least significant bit.
   */


///////////////////////////////////////////////////////////////////////////////
// Bitdefs


#define SMALLBIT                  MSB               // marks a small node
#define NOTHING                   MSB               // Marks a nil referenc
#define MAXINDEX                  (MSB - 1)         // Second biggest value
#define MAXDISTANCE               MAXINDEX          // maximal distance value
#define MAXTEXTLEN                ((MAXINDEX / ((LARGE_VERTEXSIZE+SMALL_VERTEXSIZE) / 2)) - 3)
#define UNDEF                      (~((Uint) 0))    // All ones


///////////////////////////////////////////////////////////////////////////////
// Queries

// An extra bit
// with each such integer tells whereth the reference is to a leaf or to a
// branching vertex
#define IS_LEAF(V)          ((V) & LEAFBIT)
#define IS_SMALL(V)         (((V) & SMALLBIT))
#define IS_LARGE(V)         (!((V) & SMALLBIT))
#define IS_LAST(C)          ((C) >= sentinel)
#define IS_SENTINEL(C)      ((C) == sentinel)
#define IS_NOTHING(P)       ((P) & NOTHING)
#define IS_SOMETHING(P)     (!IS_NOTHING((P)))
#define IS_ROOT(ST, V)      ((ST)->inner.first == V)
#define IS_UNDEF(V)         ((V) == UNDEF)
#define IS_HEAD_A_VERTEX    (stree->headedge.end == NULL)
#define IS_HEAD_ROOTEDGE    (stree->headedge.depth == 0)
#define IS_HEAD_ROOT        IS_HEAD_ROOTEDGE && IS_HEAD_A_VERTEX
#define IS_HEAD_EMPTY       (stree->headedge.start == stree->headedge.end)
#define IS_NO_SPACE         (stree->inner.next >= stree->allocated)
#define IS_LEFTMOST(V)      ((V) == UNDEF)

///////////////////////////////////////////////////////////////////////////////
// Getters

// The vertices are be implemented as a collection of integers.
//
// For each small vertex there is a record that stores the distance,
// child and sibling. For each large vertex w there is a record that stores
// child, sibling, depth and head.
//
// If depth is smaller than some constant K, then it is said that the large
// record is complete. A complete large record also stores the suffix link
//
// The root is refereced by the first inner vertex
#define ROOT(ST)            ((ST)->inner.first)
#define ROOT_CHILD(C)           (stree->rootchildren[(Uint) (C)])

// Leaves
#define LEAF_SIBLING(V)        (*(V))
#define LEAF_NUMBER(V)         ((V) & ~(LEAFBIT | SMALLBIT))
#define LEAF_VERTEX(ST, N)     (ST)->leaves.first[(N)]              // Value

// Inner
#define CHILD(V)               ((*(V)) & MAXINDEX)  // Remove the MSB
#define SIBLING(V)             (*((V) + 1))
#define INNER(ST, V)           (ST)->inner.first + LEAF_NUMBER((V)) // address

// Small inner
#define DISTANCE(V)            (*((V) + 2))
#define CHAIN_END(V, D)        (V) + SMALL_VERTEXSIZE * (D)

// Large inner
#define DEPTH(V)               (*((V) + 2))
#define HEAD(V)                (*((V) + 3))
#define SUFFIX_LINK(V)         (*((V) + 4))

// The label for a incoming edge to a vertex wu can be obtained by dropping
// depth(w) characters of wu.
#define LABEL_START(ST, O)        text + (O)

#define START_ALLOCSIZE         max(0.5 * SMALL_VERTEXSIZE * (textlen + 1), 48);
#define EXTRA_ALLOCSIZE         max(0.05 * SMALL_VERTEXSIZE * (textlen + 1), 48);
// Index of a branch and leaf relative to the first address
#define INDEX(A)                ((Uint) ((A) - ROOT(stree)))

///////////////////////////////////////////////////////////////////////////////
// Constructors

#define MAKE_LEAF(V)               ((V) | LEAFBIT)    // indicate this is a leaf
#define MAKE_LARGE(V)              (V)
#define MAKE_LARGE_LEAF(V)         MAKE_LEAF(V)

///////////////////////////////////////////////////////////////////////////////
// Setters

#define SET_CHILD(V, VAL)                *(V) = ((*(V)) & SMALLBIT) | (VAL)
#define SET_SIBLING(B, VAL)              *(B + 1) = VAL
// The only thing a leaf stores is a reference to its right sibling. Therefore
// to set a leaf sibling, simply set the value of the previous one to the
// current one.
//
//
// The SET_DISTANCE macro is used when defining the chain of small nodes. For
// each of the small nodes, they also set the small bit.
#define SET_DISTANCE(V, VAL)             *(V + 2) = VAL; *(V) = (*(V)) | SMALLBIT
#define SET_SUFFIXLINK(SL)              *(stree->inner.next + 4) = (SL)
#define SET_CHILD_AND_SIBLING(B, C, S)  SET_CHILD(B, C); SET_SIBLING(B, S)
#define SET_DEPTH(D)                    *(stree->inner.next + 2) = D
#define SET_HEAD(H)                     *(stree->inner.next + 3) = H
#define SET_ROOTCHILD(I, C)             (stree->rootchildren[(Uint) (I)]) = (C)
#define SET_LEAF_SIBLING(L, S)          *(L) = (S)


// Get info for branch vertex
Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

void update_chain(STree *stree, Uint *vertexp, Uint **largep, Uint *distance);

// The smallest integer i such that vertexp = head(i)
Uint get_head(STree *stree, Uint *vertexp, Uint **largep, Uint distance);

Uint get_depth(STree *stree, Uint *vertexp, Uint distance, Uint **largep);

void follow_link(STree *stree);

void set_child_and_sibling(STree *stree, Uint child, Uint sibling);

#endif

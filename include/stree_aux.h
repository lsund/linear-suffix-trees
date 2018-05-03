#ifndef STREE_AUX_H
#define STREE_AUX_H

#include "streedata.h"
#include "bitvector.h"

#define SMALL_WIDTH           3                   // # of integers for small node
#define LARGE_WIDTH           5                   // # of integers for large node
#define DIV_SMALL_WIDTH(V)   ((V) / SMALL_WIDTH)  // div by SMALL_WIDTH
#define MULT_SMALL_WIDTH(V)  ((V) * SMALL_WIDTH)  // multiply by SMALL_WIDTH


/*
   We use the least significant bit to discriminate references to leafs
   and branching nodes. Since base addresses are even, the unset least
   significant bit of a reference identifies a base address. For a leaf
   reference we shift the leaf number one to the right and
   set the least significant bit.
   */


///////////////////////////////////////////////////////////////////////////////
// Bitdefs


#define SMALLBIT            FIRSTBIT       // mark small node
#define NOTHING              FIRSTBIT       // mark nil reference in brother
#define MAXINDEX            (NOTHING - 1)   // all except for first bit
#define MAXDISTANCE         MAXINDEX       // maximal distance value
#define MAXTEXTLEN                ((MAXINDEX / ((LARGE_WIDTH+SMALL_WIDTH) / 2)) - 3)


///////////////////////////////////////////////////////////////////////////////
// Queries

#define IS_HEAD_EMPTY           (stree->headstart == stree->headend)
#define IS_LARGE(V)                (!((V) & SMALLBIT))
#define IS_LAST(C)          ((C) >= sentinel)
#define IS_LEAF(V)                 ((V) & LEAFBIT)
#define IS_NOTHING(P)                 ((P) & NOTHING)
#define IS_ROOT(ST, V)          ((ST)->inner.first == V)
#define IS_UNDEF(V)          ((V) == UNDEFREFERENCE)
#define IS_HEAD_VERTEX      (stree->headend == NULL)
#define IS_ROOT_DEPTH       (stree->head_depth == 0)
#define IS_HEAD_ROOT        (IS_ROOT_DEPTH && IS_HEAD_VERTEX)

///////////////////////////////////////////////////////////////////////////////
// Getters

#define CHAIN_END(B, D)      (B) + MULT_SMALL_WIDTH(D)
#define DEPTH(B)               (*((B) + 2))
#define DISTANCE(B)            (*((B) + 2))
#define HEAD(B)                (*((B) + 3))
#define START_ALLOCSIZE         max(0.5 * MULT_SMALL_WIDTH(textlen + 1), 48);
#define EXTRA_ALLOCSIZE         max(0.05 * MULT_SMALL_WIDTH(textlen + 1), 48);
#define LABEL_START(ST, O)        text + (O)
#define LEAF_NUM(V)           ((V) & ~(LEAFBIT | SMALLBIT))
#define LEAF_REF(ST, V)    (ST)->inner.first + LEAF_NUM((V))
#define LEAF_VERTEX(ST, N) (ST)->leaf_vertices.first[(N)]
#define CHILD(B)               ((*(B)) & MAXINDEX)
#define SIBLING(B)             (*((B)+1))
#define ROOT_CHILD(ST, C)   ((ST)->rootchildren[(Uint) (C)])
#define ROOT(ST)            ((ST)->inner.first)
// Index of a branch and leaf relative to the first address
#define INDEX(ST,A)      ((Uint) ((A) - ROOT(ST)))


///////////////////////////////////////////////////////////////////////////////
// Constructors

#define MAKE_LEAF(V)               ((V) | LEAFBIT)
#define MAKE_LARGE(V)              (V)
#define MAKE_LARGE_LEAF(V)          MAKE_LEAF(V)
#define UNDEF            (~((Uint) 0))

///////////////////////////////////////////////////////////////////////////////
// Setters

#define SET_CHILD(B,VAL)                *(B) = ((*(B)) & SMALLBIT) | (VAL)
#define SET_SIBLING(B,VAL)               *(B + 1) = VAL
#define SET_LEAF_SIBLING(B,VAL)           *(B) = (VAL)
#define SET_DISTANCE(B,VAL)              *(B + 2) = VAL; *(B) = (*(B)) | SMALLBIT
#define SET_SUFFIXLINK(SL)               *(stree->inner.next+4) = (SL)
#define SET_CHILD_AND_SIBLING(B, C, S)  SET_CHILD(B, C); SET_SIBLING(B, S)
#define SET_DEPTH(D)                    *(stree->inner.next + 2) = D
#define SET_HEAD(H)                     *(stree->inner.next + 3) = H


// Get info for branch vertex
Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

Uint get_head(STree *stree, Uint *vertexp, Uint **largep, Uint *distance);

Uint get_depth(STree *stree, Uint *vertexp, Uint *distance, Uint **largep);

Uint get_depth_after_head(STree *stree, Uint *vertexp, Uint *distance, Uint **largep);

void follow_link(STree *stree);

void set_child_and_sibling(STree *stree, Uint child, Uint sibling);

#endif

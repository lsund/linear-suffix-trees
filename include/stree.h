#ifndef STREE_H
#define STREE_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>
#include <limits.h>
#include <stdbool.h>

#include "config.h"
#include "clock.h"
#include "text.h"
#include "types.h"
#include "streedef.h"
#include "init.h"
#include "util.h"

// Constant representing an undefined value
#define UNDEF                     (~((Uint) 0))
#define NOTHING                   MSB               // Marks a nil referenc

// The number of integers required to represent a leaf.
#define LEAF_VERTEXSIZE             1
// The number of integers required to represent a small inner vertex.
#define SMALL_VERTEXSIZE            3
// The number of integers required to represent a large inner vertex.
#define LARGE_VERTEXSIZE            5

// The value represented by the most significant bit of an unsigned integer.
#define MSB                         (UintConst(1) << (INTWORDSIZE - 1))
// The value represented by the second most significant bit of an unsigned
// integer.
#define SECOND_MSB                  (MSB >> 1)

// The initial amount of memory allocated by the program.
#define START_ALLOCSIZE         max(0.5 * SMALL_VERTEXSIZE * (text.len + 1), 48);

// The additional memory the program allocates when needed.
#define EXTRA_ALLOCSIZE         max(0.05 * SMALL_VERTEXSIZE * (text.len + 1), 48);

///////////////////////////////////////////////////////////////////////////////
// Query and modify vertices

// The label for a incoming edge to a vertex wu can be obtained by dropping
// depth(w) characters of wu.
#define LABEL_START(O)          text.fst + (O)

///////////////////////////////////////////////////////////////////////////////
// Queries

// An extra bit
// with each such integer tells whereth the reference is to a leaf or to a
// branching vertex
#define IS_LEAF(V)                  ((V) & SECOND_MSB)
#define IS_SMALL(V)                 ((V) & MSB)
#define IS_LARGE(V)                 (!((V) & MSB))
#define IS_LAST(C)                  ((C) >= text.lst)
#define IS_ROOT(V)                  (st->is.fst == V)
#define EXISTS(V)                   ((V) != UNDEF)

///////////////////////////////////////////////////////////////////////////////
// Vertices

#define ROOT                    (st->is.fst)
#define ROOT_CHILD(C)           (st->rs[(Uint) (C)])
// Index of a reference
#define REF_TO_INDEX(P)         ((Uint) ((P) - st->is.fst))
#define LEAFREF_TO_INDEX(P)     ((Uint) ((P) - st->ls.fst))

// The reference to this specific vertex
#define VERTEX_TO_REF(V)        st->is.fst + VERTEX_TO_INDEX((V)) // address
#define VERTEX_TO_LEAFREF(V)    st->ls.fst + VERTEX_TO_INDEX((V))

// Make a leaf vertex, or a small vertex
#define MAKE_LEAF(V)            ((V) | SECOND_MSB)
#define MAKE_SMALL(V)           ((V) | MSB)

// LEAF
#define VERTEX_TO_INDEX(V)    ((V) & ~(MSB | SECOND_MSB))
#define LEAF_SIBLING(P)        (*(P))

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

// Additionally sets the child bit for the parent
#define SET_CHILD(V, VAL)               *(V) = ((*(V)) & MSB) | (VAL)

bool base_is_vertex(STree *st);

bool is_head_old(STree *st);

// Get info for branch vertex
Uint get_depth_head(STree *st, Uint *depth, Uint *head, Uint *v, Uint *largep);

void set_dist_and_chainend(STree *st, Uint *v, Uint **end, Uint *dist);

// The smallest integer i such that v = head(i)
Uint get_headpos(STree *st, Uint *v, Uint **largep, Uint distance);

Uint get_depth(STree *st, Uint *v, Uint distance, Uint **largep);

void follow_link(STree *st);

void set_child_and_sibling(STree *st, Uint child, Uint sibling);

bool tail_at_lastchar(STree *st);

bool head_is_root(STree *st);

VertexP get_next_inner(STree *st);

Uint get_next_leafnum(STree *st);

void set_next_leaf(STree *st, Vertex v);

#endif

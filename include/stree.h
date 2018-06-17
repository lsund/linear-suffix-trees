/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#ifndef STREE_H
#define STREE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>
#include <limits.h>
#include <stdbool.h>

#include "basedef.h"
#include "clock.h"
#include "externs.h"
#include "types.h"
#include "arraydef.h"
#include "bitvector.h"
#include "streedef.h"
#include "bitvector.h"
#include "init.h"

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

#define LEAFBIT                   SECOND_MSB        // mark leaf address
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
#define IS_NOTHING(P)               ((P) & NOTHING)
#define IS_SOMETHING(P)             (!IS_NOTHING((P)))
#define IS_ROOT(V)                  (st->is.fst == V)
#define EXISTS(V)                   ((V) != UNDEF)

///////////////////////////////////////////////////////////////////////////////
// Vertices

#define ROOT                    (st->is.fst)
#define ROOT_CHILD(C)           (st->rootchildren[(Uint) (C)])
// Index of a reference
#define INNERREF_TO_INDEX(P)    ((Uint) ((P) - st->is.fst))
#define LEAFREF_TO_INDEX(P)     ((Uint) ((P) - st->ls.fst))

// The reference to this specific vertex
#define VERTEX_TO_INNERREF(V)    st->is.fst + VERTEX_TO_INDEX((V)) // address
#define VERTEX_TO_LEAFREF(V)     st->ls.fst + VERTEX_TO_INDEX((V))

// Make a leaf vertex, or a small vertex
#define MAKE_LEAF(V)            ((V) | LEAFBIT)
#define MAKE_SMALL(V)           ((V) | SMALLBIT)

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

#define SET_ROOTCHILD(I, C)             (st->rootchildren[(Uint) (I)]) = (C)

// Additionally sets the child bit for the parent
#define SET_CHILD(V, VAL)               *(V) = ((*(V)) & MSB) | (VAL)

Bool base_is_vertex(STree *st);

Bool is_head_old(STree *st);

// Get info for branch vertex
Uint get_depth_head(STree *st, Uint *depth, Uint *head, Uint *vertexp, Uint *largep);

void update_chain(STree *st, Uint *vertexp, Uint **largep, Uint *distance);

// The smallest integer i such that vertexp = head(i)
Uint get_headpos(STree *st, Uint *vertexp, Uint **largep, Uint distance);

Uint get_depth(STree *st, Uint *vertexp, Uint distance, Uint **largep);

void follow_link(STree *st);

void set_child_and_sibling(STree *st, Uint child, Uint sibling);
Bool tail_at_lastchar(STree *st);

Bool head_is_root(STree *st);

void stree_free(STree *st);

VertexP get_next_inner(STree *st);

Uint get_next_leafnum(STree *st);

void set_next_leaf(STree *st, Vertex v);

#endif

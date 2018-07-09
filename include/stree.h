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
#include "chain.h"

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

// Mark a vertex to be a leaf
#define MAKE_LEAF(V)            ((V) | SECOND_MSB)
// Mark a vertex to be a small, inner vertex
#define MAKE_SMALL(V)           ((V) | MSB)

// Is this vertex a leaf?
#define IS_LEAF(V)                  ((V) & SECOND_MSB)
// Is this vertex a small, inner vertex?
#define IS_SMALL(V)                 ((V) & MSB)
// Is this vertex a large vertex?
#define IS_LARGE(V)                 (!((V) & MSB))
// Is this vertex the root?
#define IS_ROOT(V)                  (st->is.fst == V)
// Is this vertex not undefined?
#define EXISTS(V)                   ((V) != UNDEF)

// The index of a vertex is obtained by stripping of the most and second most
// significant bits.
#define VERTEX_TO_INDEX(V)     ((V) & ~(MSB | SECOND_MSB))

// A leaf directly stores its sibling
#define LEAF_SIBLING(P)        (*(P))

// An inner vertex stores its leftmost child ...
#define CHILD(P)               ((*(P)) & ~(MSB))
// ... and its sibling.
#define SIBLING(P)             (*((P) + 1))
// Small inner vertices store the distance to the last vertex in the chain.
#define DISTANCE(P)            (*((P) + 2))
// Large inner vertices store their label-depth in the tree ...
#define DEPTH(P)               (*((P) + 2))
// ... their head position ...
#define HEADPOS(P)             (*((P) + 3))
// ... and suffix link.
#define SUFFIX_LINK(P)         (*((P) + 4))

// Additionally sets the child bit for the parent
#define SET_CHILD(V, VAL)               *(V) = ((*(V)) & MSB) | (VAL)

// Is the base location w, a vertex?
bool base_is_vertex(STree *st);

// Was the head created by some iteration prior to the last?
bool is_head_old(STree *st);

// Does the tail point to the last character in `text`?
bool tail_at_lastchar(STree *st);

bool head_is_root(STree *st);

// The head position of the vertex `v`. The head position is the smallest integer
// i such that `v` = head(i).
Uint get_headpos(STree *st, VertexP v, Uint dist, VertexP chain_term);

// The label-depth of vertex `v`. This is the length of the string that the
// vertex corresponds to in the tree.
Uint get_depth(STree *st, VertexP v, Uint dist, VertexP chain_term);

// Sets the current head to the suffix link of the current head.
void set_head_to_suffixlink(STree *st);

// Sets the distance for vertex `v` and the chain terminator depending on this
// distance.
void set_dist_and_chainterm(STree *st, Uint *v, Uint **end, Uint *dist);

#endif

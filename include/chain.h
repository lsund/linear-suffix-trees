#ifndef CHAIN_H
#define CHAIN_H

#include "stree.h"

void update_chain(STree *st, Uint *vertexp, Uint **chainend, Uint *distance);

void reset_chain(STree *st);

void init_chain(STree *st);

void set_chain_distances(STree *st);

void append_chain(STree *st);

#endif


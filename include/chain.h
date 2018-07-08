#ifndef CHAIN_H
#define CHAIN_H

#include "stree.h"


// Reset the fields of the chain to default values
void reset_chain(STree *st);

// Grow a chain by adding a small vertex
void grow_chain(STree *st);

// Finalize a chain. This procedure first sets the distances for all small
// vertices in the chain accordingly, then adds a large vertex to the chain and
// resets it.
void finalize_chain(STree *st);

#endif


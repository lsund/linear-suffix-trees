#ifndef INIT_H
#define INIT_H

#include "stree.h"
#include "chain.h"
#include "get_alpha.h"


// Initializes the suffix tree pointed to by `st` by allocating sufficient
// space and setting its fields to default, sound values.
void init(STree *st);

// Destroys the suffix tree pointed to by `st` by freeing its allocated memory.
void destroy(STree *st);

#endif

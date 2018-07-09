#ifndef SCAN_H
#define SCAN_H

#include "types.h"
#include "stree.h"
#include "config.h"
#include "util.h"
#include "location.h"
#include "chain.h"
#include "text.h"

// Scans a prefix of the current tail from the current base vertex
void scan_tail(STree *st);

#define MATCHED(X, R, L)        (X) == (Uint) ((R) - (L) + 1)

#endif

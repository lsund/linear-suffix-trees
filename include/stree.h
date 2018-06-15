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
#include "spaceman.h"
#include "clock.h"
#include "externs.h"
#include "types.h"
#include "arraydef.h"
#include "bitvector.h"
#include "stree_aux.h"
#include "init.h"

void stree_free(STree *stree);

VertexP next_inner(STree *stree);

#endif

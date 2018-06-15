/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include "stree.h"

Wchar *text;
Wchar *sentinel;
Uint textlen, alphasize;

VertexP next_inner(STree *stree)
{
    return stree->inner.first + stree->inner.next_ind;
}

VertexP next_leaf(STree *stree)
{
    return stree->leaves.first + stree->leaves.next_ind;
}


void stree_free(STree *stree)
{
    FREE(stree->leaves.first);
    FREE(stree->rootchildren);
    FREE(stree->inner.first);
}



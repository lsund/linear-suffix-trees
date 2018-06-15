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

VertexP get_next_inner(STree *st)
{
    return st->inner.first + st->inner.next_ind;
}


VertexP get_next_leaf(STree *st)
{
    return st->leaves.first + st->leaves.next_ind;
}


Uint get_next_leafnum(STree *st)
{
    return st->leaves.next_ind | LEAFBIT;
}


void set_next_leaf(STree *st, Vertex v)
{
    *st->leaves.next = v;
}


void st_free(STree *st)
{
    free(st->leaves.first);
    free(st->rootchildren);
    free(st->inner.first);
}



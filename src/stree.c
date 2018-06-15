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

VertexP get_nxt_inner(STree *st)
{
    return st->is.fst + st->is.nxt_ind;
}


VertexP get_nxt_leaf(STree *st)
{
    return st->ls.fst + st->ls.nxt_ind;
}


Uint get_nxt_leafnum(STree *st)
{
    return st->ls.nxt_ind | LEAFBIT;
}


void set_nxt_leaf(STree *st, Vertex v)
{
    *st->ls.nxt = v;
}


void st_free(STree *st)
{
    free(st->ls.fst);
    free(st->rootchildren);
    free(st->is.fst);
}



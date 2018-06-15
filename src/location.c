/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */


#include "location.h"

Text text;


void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc)
{
    loc->next          = vertexp;
    loc->string.start  = head;
    loc->string.length = depth;
    loc->remain        = 0;
    loc->leafedge      = False;
}


void make_loc(STree *st, Uint leafnum, Uint plen, Loc *loc, Wchar *first)
{
    loc->string.start  = leafnum;
    loc->string.length = plen;
    loc->prev          = st->inner.first;
    loc->edgelen       = text.len - leafnum + 1;
    loc->remain        = loc->edgelen - plen;
    loc->next          = st->leaves.first + leafnum;
    loc->first         = first;
}



void update_loc(Uint *next, Uint start, Uint plen, Wchar *first, Uint depth, Uint edgelen, Loc *loc)
{
    loc->string.start  = start;
    loc->string.length = depth + plen;
    loc->prev          = loc->next;
    loc->edgelen       = edgelen;
    loc->remain        = loc->edgelen - plen;
    loc->first         = first;
    loc->next          = next;
}

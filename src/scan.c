/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include "scan.h"


///////////////////////////////////////////////////////////////////////////////
// Private


static void skip_edge(
        Loc *loc, Uint *vertexp, Pattern *patt, Uint *depth, Uint head, Uint plen, Uint edgelen)
{
    loc->string.start  = head;
    loc->string.length = *depth + plen;
    patt->start          += edgelen;
    *depth              += edgelen;
    loc->next          = vertexp;
    loc->remain        = 0;
}

static Uint prefixlen(Wchar *start, Pattern *patt, Uint remain)
{
    if (remain > 0) {
        Wchar *patt_start = patt->start + remain;
        Wchar *text_start = start + remain;
        Wchar *text_end = sentinel - 1;
        Uint lcp_res = lcp(patt_start, patt->end, text_start, text_end);
        return remain + lcp_res;
    } else {
        Wchar *patt_start = patt->start + 1;
        Wchar *text_start = start + 1;
        Wchar *text_end = sentinel - 1;
        Uint lcp_res = lcp(patt_start, patt->end, text_start, text_end);
        return 1 + lcp_res;
    }
}


static Uint  match_leaf(Loc *loc, Uint vertex, Pattern *patt, Uint remain)
{
    Uint leafnum = LEAF_NUM(vertex);
    loc->first   = text + leafnum;

    return prefixlen(loc->first, patt, remain);
}


///////////////////////////////////////////////////////////////////////////////
// Public


Wchar *scan(STree *stree, Loc *loc, Uint *start_vertex, Pattern patt)
{


    /* Wchar *patt    = patt_start; */
    Uint *vertexp   = start_vertex;
    Uint *chainend    = NULL;
    Uint head       = 0;
    Uint depth      = 0;
    Uint distance   = 0;
    Uint remain     = 0;
    Wchar firstchar = 0;
    Uint edgelen    = 0;

    if(!IS_ROOT(stree, vertexp)) {

        get_chainend(stree, vertexp, &chainend, &distance);
        head = get_head(stree, vertexp, &chainend, distance);

        get_chainend(stree, vertexp, &chainend, &distance);
        depth = get_depth(stree, vertexp, distance, &chainend);
    }

    init_loc(vertexp, head, depth, loc);

    while(True) {

        if (patt.start > patt.end) {
            return NULL;
        }

        firstchar    = *patt.start;
        Uint vertex  = 0;
        Uint leafnum = 0;
        Wchar *label = NULL;
        Uint plen    = 0;

        if(IS_ROOT(stree, vertexp)) {

            vertex = ROOT_CHILD(stree, firstchar);

            if (IS_UNDEF(vertex)) {
                return patt.start;
            }

            if(IS_LEAF(vertex)) {

                plen = match_leaf(loc, vertex, &patt, remain);

                if(MATCHED(plen, patt.end, patt.start)) {
                    return NULL;
                } else {
                    return patt.start + plen;
                }
            }

            vertexp = LEAF_REF(stree, vertex);

            get_chainend(stree, vertexp, &chainend, &distance);
            head = get_head(stree, vertexp, &chainend, distance);

            label   = LABEL_START(stree, head);

        } else {

            Wchar edgechar;
            vertex = CHILD(vertexp);

            while(True) {

                if (IS_NOTHING(vertex)) {

                    return patt.start;

                } else if (IS_LEAF(vertex)) {

                    leafnum = LEAF_NUM(vertex);
                    label   = LABEL_START(stree, depth + leafnum);

                    if(IS_LAST(label)) {
                        return patt.start;
                    }

                    edgechar = *label;
                    if(edgechar > firstchar) {
                        return patt.start;
                    }

                    if(edgechar == firstchar) {

                        plen = prefixlen(label, &patt, remain);
                        if(MATCHED(plen, patt.end, patt.start)) {
                            return NULL;
                        } else {
                            return patt.start + plen;
                        }
                    }

                    vertex = LEAF_VERTEX(stree, leafnum);

                } else {

                    vertexp  = LEAF_REF(stree, vertex);

                    get_chainend(stree, vertexp, &chainend, &distance);
                    head = get_head(stree, vertexp, &chainend, distance);

                    label    = LABEL_START(stree, depth + head);
                    edgechar = *label;

                    if (edgechar > firstchar) {
                        return patt.start;
                    }

                    if(edgechar == firstchar) {
                        break;
                    }

                    vertex = SIBLING(vertexp);
                }
            }
        }

        Uint prevdepth = depth;

        get_chainend(stree, vertexp, &chainend, &distance);
        depth = get_depth(stree, vertexp, distance, &chainend);
        edgelen = depth - prevdepth;

        if(remain > 0) {
            if(remain >= edgelen) {
                plen = edgelen;
                remain -= plen;
            } else {
                Uint lcp_res = lcp(patt.start + remain, patt.end, label + remain, label + edgelen-1);
                plen = remain + lcp_res;
                remain = 0;
            }
        } else {
            plen = 1 + lcp(patt.start + 1, patt.end, label + 1, label+edgelen - 1);
        }


        if(plen == edgelen) {

            skip_edge(loc, vertexp, &patt, &prevdepth, head, plen, edgelen);

        } else {

            update_loc(vertexp, head, plen, label, prevdepth, edgelen, loc);

            if(MATCHED(plen, patt.end, patt.start)) {
                return NULL;
            }

            return patt.start + plen;
        }
    }
}

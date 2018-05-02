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


static Uint get_depth_head(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep)
{
    Uint distance = 0;
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {

        distance = 1 +
            DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - (vertexp)));
        *depth = stree->currentdepth + distance;
        *head = stree->leaf_vertices.next_free_num - distance;

    } else {

        if(ISLARGE(*(vertexp))) {

            *depth = GETDEPTH(vertexp);
            *head = GETHEADPOS(vertexp);

        } else {

            distance = GETDISTANCE(vertexp);
            GETCHAINEND(largep, vertexp, distance);
            *depth = GETDEPTH(largep) + distance;
            *head = GETHEADPOS(largep) - distance;

        }
    }
    return distance;
}


static void get_head(
                STree *stree, Uint *head, Uint *vertexp, Uint *largep, Uint *distance)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        *head = stree->leaf_vertices.next_free_num - *distance;
    } else
    {
        if(ISLARGE(*(vertexp))) {
            *head = GETHEADPOS(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(largep,vertexp, *distance);
            *head = GETHEADPOS(largep) - *distance;
        }
    }
}

static void get_depth(
        STree *stree, Uint *vertexp, Uint *distance, Uint *depth, Uint *largep)
{
    if(stree->chainstart != NULL && vertexp >= stree->chainstart) {
        *distance = 1 + DIVBYSMALLINTS((Uint) (stree->inner_vertices.next_free - vertexp));
        *depth = stree->currentdepth  + *distance;
    } else {
        if(ISLARGE(*vertexp)) {
            *depth = GETDEPTH(vertexp);
        } else {
            *distance = GETDISTANCE(vertexp);
            GETCHAINEND(largep, vertexp, *distance);
            *depth = GETDEPTH(largep) + *distance;
        }
    }
}

static void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc)
{
    loc->next          = vertexp;
    loc->string.start  = head;
    loc->string.length = depth;
    loc->remain        = 0;
}


static void make_loc(STree *stree, Uint leaf_index, Uint plen, Loc *loc, Wchar *first)
{
    loc->string.start  = leaf_index;
    loc->string.length = plen;
    loc->prev          = stree->inner_vertices.first;
    loc->edgelen       = stree->textlen - leaf_index + 1;
    loc->remain        = loc->edgelen - plen;
    loc->next          = stree->leaf_vertices.first + leaf_index;
    loc->first         = first;
}



static void update_loc(Uint *next, Uint start, Uint plen, Wchar *first, Uint depth, Uint edgelen, Loc *loc)
{
    loc->string.start  = start;
    loc->string.length = depth + plen;
    loc->prev          = loc->next;
    loc->edgelen       = edgelen;
    loc->remain        = loc->edgelen - plen;
    loc->first         = first;
    loc->next          = next;
}


static void skip_edge(
        Loc *loc, Uint *vertexp, Wchar **patt, Uint *depth, Uint head, Uint plen, Uint edgelen)
{
    loc->string.start  = head;
    loc->string.length = *depth + plen;
    *patt              += edgelen;
    *depth              += edgelen;
    loc->next          = vertexp;
    loc->remain        = 0;
}

static Uint prefixlen(STree *stree, Wchar *start, Wchar *patt, Wchar *right, Uint remain)
{
    if (remain > 0) {
        Wchar *patt_start = patt + remain;
        Wchar *patt_end = right;
        Wchar *text_start = start + remain;
        Wchar *text_end = stree->sentinel - 1;
        Uint lcp_res = lcp(patt_start, patt_end, text_start, text_end);
        return remain + lcp_res;
    } else {
        Wchar *patt_start = patt + 1;
        Wchar *patt_end = right;
        Wchar *text_start = start + 1;
        Wchar *text_end = stree->sentinel - 1;
        Uint lcp_res = lcp(patt_start, patt_end, text_start, text_end);
        return 1 + lcp_res;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


Wchar *scan(STree *stree, Loc *loc, Uint *start_vertex, Wchar *patt_start, Wchar *right)
{

    Wchar *patt    = patt_start;
    Uint *vertexp   = start_vertex;
    Uint *largep    = NULL;
    Uint head       = 0;
    Uint depth      = 0;
    Uint distance   = 0;
    Uint remain     = 0;
    Wchar firstchar = 0;
    Uint edgelen    = 0;

    if(!IS_ROOT(stree, vertexp)) {
        distance = get_depth_head(stree, &depth, &head, vertexp, largep);
    }

    init_loc(vertexp, head, depth, loc);

    while(True) {

        if (patt > right) {
            return NULL;
        }

        firstchar       = *patt;
        Uint vertex     = 0;
        Uint leaf_index = 0;
        Wchar *leftb    = NULL;
        Uint plen       = 0;

        if(IS_ROOT(stree, vertexp)) {

            vertex = ROOT_CHILD(stree, firstchar);

            if (IS_UNDEF(vertex)) {
                return patt;
            }

            if(IS_LEAF(vertex)) {

                leaf_index = LEAF_INDEX(vertex);
                loc->first = stree->text + leaf_index;

                plen = prefixlen(stree, loc->first, patt, right, remain);
                make_loc(stree, leaf_index, plen, loc, loc->first);

                if(MATCHED(plen, right, patt)) {
                    return NULL;
                }

                return patt + plen;

            }

            vertexp = stree->inner_vertices.first + LEAF_INDEX(vertex);
            get_head(stree, &head, vertexp, largep, &distance);
            leftb = stree->text + head;

        } else {

            Wchar edgechar;
            vertex = GETCHILD(vertexp);

            while(True) {

                if (IS_NOTHING(vertex)) {
                    return patt;
                }

                if (IS_LEAF(vertex)) {

                    leaf_index = LEAF_INDEX(vertex);
                    leftb = stree->text + (depth + leaf_index);

                    if(leftb == stree->sentinel) {
                        return patt;
                    }

                    edgechar = *leftb;
                    if(edgechar > firstchar) {
                        return patt;
                    }

                    if(edgechar == firstchar) {

                        Uint edgelen = stree->textlen - (depth + leaf_index) + 1;
                        Uint *next = stree->leaf_vertices.first + leaf_index;
                        plen = prefixlen(stree, leftb, patt, right, remain);
                        update_loc(next, leaf_index, plen, leftb, depth, edgelen, loc);

                        if(MATCHED(plen, right, patt)) {
                            return NULL;
                        } else {
                            return patt + plen;
                        }
                    }

                    vertex = stree->leaf_vertices.first[leaf_index];

                } else {

                    vertexp  = stree->inner_vertices.first + LEAF_INDEX(vertex);
                    get_head(stree, &head, vertexp, largep, &distance);

                    leftb    = stree->text + (depth + head);
                    edgechar = *leftb;

                    if (edgechar > firstchar) {
                        return patt;
                    }
                    if(edgechar == firstchar) {
                        break;
                    }

                    vertex = SIBLING(vertexp);
                }
            }
        }

        Uint tmpdepth;
        get_depth(stree, vertexp, &distance, &tmpdepth, largep);
        edgelen = tmpdepth - depth;

        if(remain > 0) {
            if(remain >= edgelen) {
                plen = edgelen;
                remain -= plen;
            } else {
                Uint lcp_res = lcp(patt + remain, right, leftb +remain, leftb + edgelen-1);
                plen = remain + lcp_res;
                remain = 0;
            }
        } else {
            plen = 1 + lcp(patt + 1, right, leftb + 1, leftb+edgelen - 1);
        }


        if(plen == edgelen) {

            skip_edge(loc, vertexp, &patt, &depth, head, plen, edgelen);

        } else {

            update_loc(vertexp, head, plen, leftb, depth, edgelen, loc);

            if(MATCHED(plen, right, patt)) {
                return NULL;
            }

            return patt + plen;
        }
    }
}

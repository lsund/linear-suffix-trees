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


static Uint get_both(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largep)
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


static void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc)
{
    loc->next          = vertexp;
    loc->string.start  = head;
    loc->string.length = depth;
    loc->remain        = 0;
}


static void make_loc(STree *stree, Uint leaf_index, Uint plen, Loc *loc)
{
    loc->prev          = stree->inner_vertices.first;
    loc->edgelen       = stree->textlen - leaf_index + 1;
    loc->remain        = loc->edgelen - plen;
    loc->next          = stree->leaf_vertices.first + leaf_index;
    loc->string.start  = leaf_index;
    loc->string.length = plen;
}


static Uint prefixlen(STree *stree, Loc *loc, Wchar *leftp, Wchar *right, Uint remain)
{
    if (remain > 0) {
        Wchar *patt_end = right;
        Wchar *loc_end = stree->sentinel - 1;
        Wchar *patt_start = leftp + remain;
        Wchar *loc_start = loc->first + remain;
        Uint lcp_res = lcp(patt_start, patt_end, loc_start, loc_end);
        return remain + lcp_res;
    } else {
        Wchar *patt_end = right;
        Wchar *loc_end = stree->sentinel - 1;
        Wchar *patt_start = leftp + 1;
        Wchar *loc_start = loc->first + 1;
        Uint lcp_res = lcp(patt_start, patt_end, loc_start, loc_end);
        return 1 + lcp_res;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public


Wchar *scan(STree *stree, Loc *loc, Uint * start_vertex, Wchar *start_left, Wchar *right)
{
    Uint *largep = NULL, distance = 0, plen, tmpdepth, edgelen;
    Wchar firstchar, edgechar = 0;

    Wchar *leftp = start_left;
    Uint *vertexp = start_vertex;

    Uint head, depth;
    if(IS_ROOT(stree, vertexp)) {
        depth = 0;
        head  = 0;
    } else {
        distance = get_both(stree, &depth, &head, vertexp, largep);
    }

    init_loc(vertexp, head, depth, loc);

    Uint remain = 0;

    while(True) {

        if (leftp > right) {
            return NULL;
        }

        firstchar = *leftp;

        Uint vertex     = 0;
        Uint leaf_index = 0;
        Wchar *leftb    = NULL;

        if(IS_ROOT(stree, vertexp)) {

            vertex = stree->rootchildren[(Uint) firstchar];

            if (IS_UNDEF(vertex)) {
                return leftp;
            }

            if(IS_LEAF(vertex)) {

                leaf_index = LEAF_INDEX(vertex);
                loc->first = stree->text + leaf_index;

                plen = prefixlen(stree, loc, leftp, right, remain);
                make_loc(stree, leaf_index, plen, loc);

                if(MATCHED(plen, right, leftp)) {
                    return NULL;
                }

                return leftp + plen;

            }
            vertexp = stree->inner_vertices.first + GETBRANCHINDEX(vertex);
            GETONLYHEADPOS(head, vertexp);
            leftb = stree->text + head;
        } else
        {
            vertex = GETCHILD(vertexp);
            while(True) {

                if(IS_NOTHING(vertex)) {
                    return leftp;
                }
                if(IS_LEAF(vertex))
                {
                    leaf_index = LEAF_INDEX(vertex);
                    leftb = stree->text + (depth + leaf_index);
                    if(leftb == stree->sentinel) {
                        return leftp;
                    }
                    edgechar = *leftb;
                    if(edgechar > firstchar) {
                        return leftp;
                    }
                    if(edgechar == firstchar)
                    {
                        if(remain > 0)
                        {
                            plen = remain +
                                lcp(leftp+remain,right,
                                        leftb+remain,stree->sentinel-1);
                        } else
                        {
                            plen = 1 + lcp(leftp+1,right,
                                    leftb+1,stree->sentinel-1);
                        }
                        loc->first = leftb;
                        loc->prev = loc->next;
                        loc->edgelen = stree->textlen - (depth + leaf_index) + 1;
                        loc->remain = loc->edgelen - plen;
                        loc->next = stree->leaf_vertices.first + leaf_index;
                        loc->string.start = leaf_index;
                        loc->string.length = depth + plen;
                        if(plen == (Uint) (right - leftp + 1)) {
                            return NULL;
                        }
                        return leftp + plen;
                    }
                    vertex = LEAFBROTHERVAL(stree->leaf_vertices.first[leaf_index]);
                } else
                {
                    vertexp = stree->inner_vertices.first + GETBRANCHINDEX(vertex);
                    GETONLYHEADPOS(head,vertexp);
                    leftb = stree->text + (depth + head);
                    edgechar = *leftb;
                    if (edgechar > firstchar)
                    {
                        return leftp;
                    }
                    if(edgechar == firstchar)
                    {
                        /*@innerbreak@*/ break;
                    }
                    vertex = GETBROTHER(vertexp);
                }
            }
        }
        GETONLYDEPTH(tmpdepth,vertexp);
        edgelen = tmpdepth - depth;
        if(remain > 0)
        {
            if(remain >= edgelen)
            {
                plen = edgelen;
                remain -= plen;
            } else
            {
            if (!leftb) {
                fprintf(stderr, "Not supposed to be null");
            }
                plen = remain +
                    lcp(leftp+remain,right,
                            leftb+remain,leftb+edgelen-1);
                remain = 0;
            }
        } else
        {
            if (!leftb) {
                fprintf(stderr, "Not supposed to be null");
            }
            plen = 1 + lcp(leftp+1,right,
                    leftb+1,leftb+edgelen-1);
        }
        loc->string.start = head;
        loc->string.length = depth + plen;
        if(plen == edgelen)
        {
            leftp += edgelen;
            depth += edgelen;
            loc->next = vertexp;
            loc->remain = 0;
        } else
        {
            loc->first = leftb;
            loc->prev = loc->next;
            loc->next = vertexp;
            loc->edgelen = edgelen;
            loc->remain = loc->edgelen - plen;
            if(plen == (Uint) (right - leftp + 1))
            {
                return NULL;
            }
            return leftp + plen;
        }
    }
}

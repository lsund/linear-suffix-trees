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


static Uint get_both(STree *stree, Uint *depth, Uint *head, Uint *vertexp, Uint *largeptr)
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
            GETCHAINEND(largeptr, vertexp, distance);
            *depth = GETDEPTH(largeptr) + distance;
            *head = GETHEADPOS(largeptr) - distance;

        }
    }
    return distance;
}


static void init_loc(Uint *vertexp, Uint head, Uint depth, Loc *loc)
{
    loc->nextnode         = vertexp;
    loc->locstring.start  = head;
    loc->locstring.length = depth;
    loc->remain           = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Public


Wchar *scan(STree *stree, Loc *loc, Uint * start_vertex, Wchar *left, Wchar *right)
{
    Uint *largeptr = NULL, leafindex,
         node = 0, distance = 0, prefixlen, tmpdepth,
         edgelen;
    Wchar *lptr, *leftborder = NULL, firstchar, edgechar = 0;

    lptr = left;
    Uint *vertexp = start_vertex;

    Uint head, depth;
    if(IS_ROOT(stree, vertexp)) {
        depth = 0;
        head  = 0;
    } else {
        distance = get_both(stree, &depth, &head, vertexp, largeptr);
    }

    init_loc(vertexp, head, depth, loc);

    Uint remain = 0;
    while(True) {

        if (lptr > right) return NULL;

        firstchar = *lptr;

        if(IS_ROOT(stree, vertexp)) {
            if((node = stree->rootchildren[(Uint) firstchar]) == UNDEFREFERENCE)
            {
                return lptr;
            }
            if(ISLEAF(node)) {
                leafindex = GETLEAFINDEX(node);
                loc->firstptr = stree->text + leafindex;
                if(remain > 0)
                {
                    prefixlen = remain +
                        lcp(lptr+remain,right,
                                loc->firstptr+remain,stree->sentinel-1);
                } else {
                    prefixlen = 1 + lcp(lptr+1,right,
                            loc->firstptr+1,stree->sentinel-1);
                }

                loc->previousnode = stree->inner_vertices.first;
                loc->edgelen = stree->textlen - leafindex + 1;
                loc->remain = loc->edgelen - prefixlen;
                loc->nextnode = stree->leaf_vertices.first + leafindex;
                loc->locstring.start = leafindex;
                loc->locstring.length = prefixlen;
                if(prefixlen == (Uint) (right - lptr + 1))
                {
                    return NULL;
                }
                return lptr + prefixlen;
            }
            vertexp = stree->inner_vertices.first + GETBRANCHINDEX(node);
            GETONLYHEADPOS(head, vertexp);
            leftborder = stree->text + head;
        } else
        {
            node = GETCHILD(vertexp);
            while(True)
            {
                if(NILPTR(node))
                {
                    return lptr;
                }
                if(ISLEAF(node))
                {
                    leafindex = GETLEAFINDEX(node);
                    leftborder = stree->text + (depth + leafindex);
                    if(leftborder == stree->sentinel)
                    {
                        return lptr;
                    }
                    edgechar = *leftborder;
                    if(edgechar > firstchar)
                    {
                        return lptr;
                    }
                    if(edgechar == firstchar)
                    {
                        if(remain > 0)
                        {
                            prefixlen = remain +
                                lcp(lptr+remain,right,
                                        leftborder+remain,stree->sentinel-1);
                        } else
                        {
                            prefixlen = 1 + lcp(lptr+1,right,
                                    leftborder+1,stree->sentinel-1);
                        }
                        loc->firstptr = leftborder;
                        loc->previousnode = loc->nextnode;
                        loc->edgelen = stree->textlen - (depth + leafindex) + 1;
                        loc->remain = loc->edgelen - prefixlen;
                        loc->nextnode = stree->leaf_vertices.first + leafindex;
                        loc->locstring.start = leafindex;
                        loc->locstring.length = depth + prefixlen;
                        if(prefixlen == (Uint) (right - lptr + 1)) {
                            return NULL;
                        }
                        return lptr + prefixlen;
                    }
                    node = LEAFBROTHERVAL(stree->leaf_vertices.first[leafindex]);
                } else
                {
                    vertexp = stree->inner_vertices.first + GETBRANCHINDEX(node);
                    GETONLYHEADPOS(head,vertexp);
                    leftborder = stree->text + (depth + head);
                    edgechar = *leftborder;
                    if (edgechar > firstchar)
                    {
                        return lptr;
                    }
                    if(edgechar == firstchar)
                    {
                        /*@innerbreak@*/ break;
                    }
                    node = GETBROTHER(vertexp);
                }
            }
        }
        GETONLYDEPTH(tmpdepth,vertexp);
        edgelen = tmpdepth - depth;
        if(remain > 0)
        {
            if(remain >= edgelen)
            {
                prefixlen = edgelen;
                remain -= prefixlen;
            } else
            {
            if (!leftborder) {
                fprintf(stderr, "Not supposed to be null");
            }
                prefixlen = remain +
                    lcp(lptr+remain,right,
                            leftborder+remain,leftborder+edgelen-1);
                remain = 0;
            }
        } else
        {
            if (!leftborder) {
                fprintf(stderr, "Not supposed to be null");
            }
            prefixlen = 1 + lcp(lptr+1,right,
                    leftborder+1,leftborder+edgelen-1);
        }
        loc->locstring.start = head;
        loc->locstring.length = depth + prefixlen;
        if(prefixlen == edgelen)
        {
            lptr += edgelen;
            depth += edgelen;
            loc->nextnode = vertexp;
            loc->remain = 0;
        } else
        {
            loc->firstptr = leftborder;
            loc->previousnode = loc->nextnode;
            loc->nextnode = vertexp;
            loc->edgelen = edgelen;
            loc->remain = loc->edgelen - prefixlen;
            if(prefixlen == (Uint) (right - lptr + 1))
            {
                return NULL;
            }
            return lptr + prefixlen;
        }
    }
}

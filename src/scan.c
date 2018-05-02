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

wchar_t *scan(STree *stree, Location *loc, Uint * btptr, wchar_t *left,
        wchar_t *right)
{
    Uint *nodeptr = NULL, *largeptr = NULL, leafindex, nodedepth,
         node = 0, distance = 0, prefixlen, headposition, tmpnodedepth,
         edgelen, remainingtoskip;
    wchar_t *lptr, *leftborder = NULL, firstchar, edgechar = 0;

    lptr = left;
    nodeptr = btptr;
    if(nodeptr == stree->inner_vertices.first) {
        nodedepth = 0;
        headposition = 0;
    } else {
        GETBOTH(nodedepth, headposition, nodeptr);
    }

    loc->nextnode = nodeptr;
    loc->locstring.start = headposition;
    loc->locstring.length = nodedepth;
    loc->remain = 0;

    remainingtoskip = 0;
    while(True)
    {
        if(lptr > right) {
            return NULL;
        }
        firstchar = *lptr;
        if(nodeptr == stree->inner_vertices.first)  // at the root
        {
            if((node = stree->rootchildren[(Uint) firstchar]) == UNDEFREFERENCE)
            {
                return lptr;
            }
            if(ISLEAF(node)) {
                leafindex = GETLEAFINDEX(node);
                loc->firstptr = stree->text + leafindex;
                if(remainingtoskip > 0)
                {
                    prefixlen = remainingtoskip +
                        lcp(lptr+remainingtoskip,right,
                                loc->firstptr+remainingtoskip,stree->sentinel-1);
                } else
                {
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
            nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
            GETONLYHEADPOS(headposition, nodeptr);
            leftborder = stree->text + headposition;
        } else
        {
            node = GETCHILD(nodeptr);
            while(True)
            {
                if(NILPTR(node))
                {
                    return lptr;
                }
                if(ISLEAF(node))
                {
                    leafindex = GETLEAFINDEX(node);
                    leftborder = stree->text + (nodedepth + leafindex);
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
                        if(remainingtoskip > 0)
                        {
                            prefixlen = remainingtoskip +
                                lcp(lptr+remainingtoskip,right,
                                        leftborder+remainingtoskip,stree->sentinel-1);
                        } else
                        {
                            prefixlen = 1 + lcp(lptr+1,right,
                                    leftborder+1,stree->sentinel-1);
                        }
                        loc->firstptr = leftborder;
                        loc->previousnode = loc->nextnode;
                        loc->edgelen = stree->textlen - (nodedepth + leafindex) + 1;
                        loc->remain = loc->edgelen - prefixlen;
                        loc->nextnode = stree->leaf_vertices.first + leafindex;
                        loc->locstring.start = leafindex;
                        loc->locstring.length = nodedepth + prefixlen;
                        if(prefixlen == (Uint) (right - lptr + 1)) {
                            return NULL;
                        }
                        return lptr + prefixlen;
                    }
                    node = LEAFBROTHERVAL(stree->leaf_vertices.first[leafindex]);
                } else
                {
                    nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
                    GETONLYHEADPOS(headposition,nodeptr);
                    leftborder = stree->text + (nodedepth + headposition);
                    edgechar = *leftborder;
                    if (edgechar > firstchar)
                    {
                        return lptr;
                    }
                    if(edgechar == firstchar)
                    {
                        /*@innerbreak@*/ break;
                    }
                    node = GETBROTHER(nodeptr);
                }
            }
        }
        GETONLYDEPTH(tmpnodedepth,nodeptr);
        edgelen = tmpnodedepth - nodedepth;
        if(remainingtoskip > 0)
        {
            if(remainingtoskip >= edgelen)
            {
                prefixlen = edgelen;
                remainingtoskip -= prefixlen;
            } else
            {
            if (!leftborder) {
                fprintf(stderr, "Not supposed to be null");
            }
                prefixlen = remainingtoskip +
                    lcp(lptr+remainingtoskip,right,
                            leftborder+remainingtoskip,leftborder+edgelen-1);
                remainingtoskip = 0;
            }
        } else
        {
            if (!leftborder) {
                fprintf(stderr, "Not supposed to be null");
            }
            prefixlen = 1 + lcp(lptr+1,right,
                    leftborder+1,leftborder+edgelen-1);
        }
        loc->locstring.start = headposition;
        loc->locstring.length = nodedepth + prefixlen;
        if(prefixlen == edgelen)
        {
            lptr += edgelen;
            nodedepth += edgelen;
            loc->nextnode = nodeptr;
            loc->remain = 0;
        } else
        {
            loc->firstptr = leftborder;
            loc->previousnode = loc->nextnode;
            loc->nextnode = nodeptr;
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

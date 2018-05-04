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
    patt->start        += edgelen;
    *depth             += edgelen;
    loc->next          = vertexp;
    loc->remain        = 0;
}

static Uint prefixlen(Wchar *start, Pattern *patt, Uint remain)
{
    if (remain < 0) {
        remain = 1;
    }
    Pattern textpatt  = make_patt(start + remain, sentinel - 1);
    Pattern curr_patt = make_patt(patt->start + remain, patt-> end);
    Uint lcp_res      = lcp(textpatt, curr_patt);
    return remain + lcp_res;
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

    VertexP vertexp  = start_vertex;
    VertexP chainend = NULL;
    Vertex  head     = 0;
    Uint depth       = 0;
    Uint distance    = 0;
    Uint remain      = 0;
    Wchar firstchar  = 0;
    Uint edgelen     = 0;

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
        Uint leafnum = 0;
        Wchar *label = NULL;
        Uint plen    = 0;

        if(IS_ROOT(stree, vertexp)) {

            Uint rootchild = ROOT_CHILD(firstchar);

            if (IS_UNDEF(rootchild)) {
                return patt.start;
            }

            if(IS_LEAF(rootchild)) {

                plen = match_leaf(loc, rootchild, &patt, remain);

                if(MATCHED(plen, patt.end, patt.start)) {
                    return NULL;
                } else {
                    return patt.start + plen;
                }
            }

            vertexp = LEAF_REF(stree, rootchild);

            get_chainend(stree, vertexp, &chainend, &distance);
            head = get_head(stree, vertexp, &chainend, distance);

            label   = LABEL_START(stree, head);

        } else {

            Wchar edgechar;
            Uint vertex = CHILD(vertexp);

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
                Pattern rem_patt  = patt_inc(patt, remain);
                Pattern labelpatt = make_patt(label + remain, label + edgelen - 1);
                Uint lcp_res      = lcp(rem_patt, labelpatt);
                plen              = remain + lcp_res;
                remain            = 0;
            }
        } else {
            Pattern labelpatt = make_patt(label + 1, label + edgelen - 1);
            plen = 1 + lcp(patt_inc(patt, 1), labelpatt);
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


// Scans a prefix of the current tail down from a given node
void scantail(STree *stree)
{
    VertexP vertexp = NULL;
    VertexP chainend = NULL;
    Uint leafindex;
    Uint depth;
    Uint edgelen;
    Vertex node;
    Uint distance = 0;
    Uint prevnode;
    Uint prefixlen;
    Uint head;
    Wchar *leftborder = (Wchar *) NULL;
    Wchar firstchar;
    Wchar edgechar = 0;

    if(IS_ROOT_DEPTH) {

        // There is no sentinel
        if(IS_SENTINEL(stree->tailptr)) {
            stree->headend = NULL;
            return;
        }

        firstchar = *(stree->tailptr);
        node = ROOT_CHILD(firstchar);
        if(node  == UNDEF) {
            stree->headend = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(node)) {

            Pattern edgepatt = make_patt(text + LEAF_NUM(node) + 1, sentinel - 1);
            Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
            prefixlen = 1 + lcp(edgepatt, tailpatt);
            (stree->tailptr) += prefixlen;
            stree->headstart = edgepatt.start - 1;
            stree->headend = edgepatt.start - 1 + (prefixlen-1);
            stree->insertnode = node;

            return;
        }
        vertexp = stree->inner.first + LEAF_NUM(node);

        get_chainend(stree, vertexp, &chainend, &distance);
        head = get_head(stree, vertexp, &chainend, distance);
        depth = get_depth(stree, vertexp, distance, &chainend);

        leftborder = text + head;

        Pattern edgepatt = make_patt(leftborder + 1, leftborder + depth - 1);
        Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
        prefixlen = 1 + lcp(edgepatt, tailpatt);

        (stree->tailptr)+= prefixlen;
        if(depth > prefixlen)   // cannot reach the successor, fall out of tree
        {
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen - 1);
            stree->insertnode = node;
            return;
        }
        stree->headnode = vertexp;
        stree->head_depth = depth;
    }
    while(True)  // \emph{headnode} is not the root
    {
        prevnode = UNDEF;
        node = CHILD(stree->headnode);
        if(stree->tailptr == sentinel)  //  \$-edge
        {
            do // there is no \$-edge, so find last successor, of which it becomes right brother
            {
                prevnode = node;
                if(IS_LEAF(node))
                {
                    node = stree->leaves.first[LEAF_NUM(node)];
                } else
                {
                    node = SIBLING(stree->inner.first + LEAF_NUM(node));
                }
            } while(!IS_NOTHING(node));
            stree->insertnode = NOTHING;
            stree->insertprev = prevnode;
            stree->headend = NULL;
            return;
        }
        firstchar = *(stree->tailptr);

        do // find successor edge with firstchar = firstchar
        {
            if(IS_LEAF(node))   // successor is leaf
            {
                leafindex = LEAF_NUM(node);
                leftborder = text + (stree->head_depth + leafindex);
                if((edgechar = *leftborder) >= firstchar)   // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = stree->leaves.first[leafindex];
            } else  // successor is branch node
            {
                vertexp = stree->inner.first + LEAF_NUM(node);

                get_chainend(stree, vertexp, &chainend, &distance);
                head = get_head(stree, vertexp, &chainend, distance);

                leftborder = text + (stree->head_depth + head);
                if((edgechar = *leftborder) >= firstchar)  // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = SIBLING(vertexp);
            }
        } while(!IS_NOTHING(node));
        if(IS_NOTHING(node) || edgechar > firstchar)  // edge not found
        {
            stree->insertprev = prevnode;   // new edge will become brother of this
            stree->headend = NULL;
            return;
        }
        if(IS_LEAF(node)) {
            // correct edge is leaf edge, compare its label with tail
            Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
            Pattern edgepatt = make_patt(leftborder + 1, sentinel - 1);

            prefixlen = 1 + lcp(tailpatt, edgepatt);

            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        depth = get_depth(stree, vertexp, distance, &chainend);
        edgelen = depth - stree->head_depth;

        Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
        Pattern edgepatt = make_patt(leftborder + 1, leftborder + edgelen - 1);

        prefixlen = 1 + lcp(tailpatt, edgepatt);

        (stree->tailptr) += prefixlen;
        if(edgelen > prefixlen)  // cannot reach next node
        {
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        stree->headnode = vertexp;
        stree->head_depth = depth;
    }
}


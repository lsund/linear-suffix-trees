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
    loc->prev          = loc->next;
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
    Uint leafnum = INDEX(vertex);
    loc->first   = text + leafnum;

    return prefixlen(loc->first, patt, remain);
}


static void find_last_successor(STree *stree, Vertex *prev_p, Vertex *vertex_p)
{
    Vertex vertex = *vertex_p;
    Vertex prev = *prev_p;
    do {
        prev = vertex;
        if(IS_LEAF(vertex)) {
            vertex = LEAF_SIBLING(stree->leaves.first + INDEX(vertex));
        } else {
            vertex = SIBLING(stree->inner.first + INDEX(vertex));
        }
    } while(IS_SOMETHING(vertex));
    *vertex_p = vertex;
    *prev_p = prev;
}


static void update_stree(STree *stree, Wchar *label_start, Uint plen, Uint scanprobe_val, Uint prev)
{
    stree->head.label.start = label_start;
    stree->head.label.end = label_start + (plen-1);
    stree->split_vertex = scanprobe_val;
    stree->insertprev = prev;
}


static Uint tail_prefixlen(STree *stree, Wchar *start, Wchar *end)
{
    Pattern tailpatt = make_patt(stree->tail + 1, sentinel - 1);
    return 1 + lcp(tailpatt, make_patt(start, end));
}


static Wchar get_label(STree *stree, Uint offset, Wchar **label_start)
{
    *label_start = text + (stree->head.depth + offset);
    return **label_start;
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

    if(!IS_ROOT(vertexp)) {

        update_chain(stree, vertexp, &chainend, &distance);
        head = get_headpos(stree, vertexp, &chainend, distance);

        update_chain(stree, vertexp, &chainend, &distance);
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

        if(IS_ROOT(vertexp)) {

            Vertex rootchild = ROOT_CHILD(firstchar);

            if (IS_UNDEF(rootchild)) {
                return patt.start;
            }

            if(IS_LEAF(rootchild)) {

                plen = match_leaf(loc, rootchild, &patt, remain);

                loc->leafedge = true;
                if(MATCHED(plen, patt.end, patt.start)) {
                    return NULL;
                } else {
                    return patt.start + plen;
                }
            }

            vertexp = INNER(rootchild);

            update_chain(stree, vertexp, &chainend, &distance);
            head = get_headpos(stree, vertexp, &chainend, distance);

            label   = LABEL_START(head);

        } else {

            Wchar labelchar;
            Uint vertex = CHILD(vertexp);

            while(True) {

                if (IS_NOTHING(vertex)) {

                    return patt.start;

                } else if (IS_LEAF(vertex)) {

                    leafnum = INDEX(vertex);
                    label   = LABEL_START(depth + leafnum);

                    if(IS_LAST(label)) {
                        return patt.start;
                    }

                    labelchar = *label;
                    if(labelchar > firstchar) {
                        return patt.start;
                    }

                    if(labelchar == firstchar) {

                        loc->leafedge = true;
                        plen = prefixlen(label, &patt, remain);
                        if(MATCHED(plen, patt.end, patt.start)) {
                            return NULL;
                        } else {
                            loc->next = LEAF(vertex);
                            return patt.start + plen;
                        }
                    }

                    vertex = LEAF_SIBLING(LEAF(leafnum));

                } else {

                    vertexp  = INNER(vertex);

                    update_chain(stree, vertexp, &chainend, &distance);
                    head = get_headpos(stree, vertexp, &chainend, distance);

                    label    = LABEL_START(depth + head);
                    labelchar = *label;

                    if (labelchar > firstchar) {
                        return patt.start;
                    }

                    if(labelchar == firstchar) {
                        break;
                    }

                    vertex = SIBLING(vertexp);
                }
            }
        }

        Uint prevdepth = depth;

        update_chain(stree, vertexp, &chainend, &distance);
        depth = get_depth(stree, vertexp, distance, &chainend);
        edgelen = depth - prevdepth;
        loc->edgelen = edgelen;

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
void scan_tail(STree *stree)
{
    VertexP scanprobe = NULL;
    VertexP chainend = NULL;
    Uint leafindex;
    Uint depth;
    Uint edgelen;
    Vertex scanprobe_val;
    Uint distance = 0;
    Uint prev;
    Uint plen;
    Uint head;
    Wchar *label_start = NULL;
    Wchar firstchar;
    Wchar labelchar = 0;

    if(head_depth(stree) == 0) {

        // There is no sentinel
        if(IS_SENTINEL(stree->tail)) {
            stree->head.label.end = NULL;
            return;
        }

        firstchar = *(stree->tail);
        scanprobe_val = ROOT_CHILD(firstchar);
        if(scanprobe_val == UNDEF) {

            stree->head.label.end = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(scanprobe_val)) {

            Pattern edgepatt = make_patt(text + INDEX(scanprobe_val) + 1, sentinel - 1);
            Pattern tailpatt = make_patt(stree->tail + 1, sentinel - 1);
            plen = 1 + lcp(edgepatt, tailpatt);

            stree->tail += plen;
            stree->head.label.start  = edgepatt.start - 1;
            stree->head.label.end    = edgepatt.start - 1 + (plen-1);
            stree->split_vertex = scanprobe_val;

            return;
        }

        scanprobe = stree->inner.first + INDEX(scanprobe_val);

        update_chain(stree, scanprobe, &chainend, &distance);
        head = get_headpos(stree, scanprobe, &chainend, distance);
        depth = get_depth(stree, scanprobe, distance, &chainend);

        label_start = text + head;
        plen = tail_prefixlen(stree, label_start + 1, label_start + depth - 1);

        stree->tail+= plen;
        if(depth > plen) {

            // cannot reach the successor, fall out of tree
            stree->split_vertex     = scanprobe_val;
            stree->head.label.start = label_start;
            stree->head.label.end   = label_start + (plen - 1);
            return;
        }
        stree->head.origin = scanprobe;
        stree->head.depth = depth;
    }

    // Head is not the root
    while(True) {
        prev = UNDEF;
        scanprobe_val = CHILD(stree->head.origin);
        if(IS_SENTINEL(stree->tail)) {
            find_last_successor(stree, &prev, &scanprobe_val);
            stree->split_vertex = NOTHING;
            stree->insertprev   = prev;
            stree->head.label.end      = NULL;
            return;
        }
        firstchar = *(stree->tail);

        do {
            // find successor edge with firstchar = firstchar
            if(IS_LEAF(scanprobe_val)) {

                leafindex = INDEX(scanprobe_val);
                labelchar = get_label(stree, leafindex, &label_start);

                if(labelchar >= firstchar) break;

                prev          = scanprobe_val;
                scanprobe_val = LEAF_SIBLING(stree->leaves.first + leafindex);

            } else {

                scanprobe   = stree->inner.first + INDEX(scanprobe_val);
                update_chain(stree, scanprobe, &chainend, &distance);

                head      = get_headpos(stree, scanprobe, &chainend, distance);
                labelchar = get_label(stree, head, &label_start);

                if (labelchar >= firstchar) break;

                prev          = scanprobe_val;
                scanprobe_val = SIBLING(scanprobe);
            }
        } while(IS_SOMETHING(scanprobe_val));

        if(IS_NOTHING(scanprobe_val) || labelchar > firstchar) {
            // edge not found
            // new edge will become brother of this
            stree->insertprev = prev;
            stree->head.label.end = NULL;
            return;
        }

        if(IS_LEAF(scanprobe_val)) {
            plen = tail_prefixlen(stree, label_start + 1, sentinel - 1);
            (stree->tail) += plen;
            update_stree(stree, label_start, plen, scanprobe_val, prev);
            return;
        }

        depth   = get_depth(stree, scanprobe, distance, &chainend);
        edgelen = depth - stree->head.depth;
        plen    = tail_prefixlen(stree, label_start + 1, label_start + edgelen - 1);
        (stree->tail) += plen;

        // cannot reach next node
        if(edgelen > plen) {
            update_stree(stree, label_start, plen, scanprobe_val, prev);
            return;
        }

        stree->head.origin = scanprobe;
        stree->head.depth = depth;
    }
}

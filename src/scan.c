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
    Uint leafnum = LEAF_NUMBER(vertex);
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
            vertex = LEAF_SIBLING(stree->leaves.first + LEAF_NUMBER(vertex));
        } else {
            vertex = SIBLING(stree->inner.first + LEAF_NUMBER(vertex));
        }
    } while(IS_SOMETHING(vertex));
    *vertex_p = vertex;
    *prev_p = prev;
}


static void update_stree(STree *stree, Wchar *label_start, Uint plen, Uint scanprobe_val, Uint prev)
{
    stree->head_start = label_start;
    stree->head_end = label_start + (plen-1);
    stree->split_vertex = scanprobe_val;
    stree->insertprev = prev;
}


static Uint tail_prefixlen(STree *stree, Wchar *start, Wchar *end)
{
    Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
    return 1 + lcp(tailpatt, make_patt(start, end));
}


static Wchar get_label(STree *stree, Uint offset, Wchar **label_start)
{
    *label_start = text + (stree->head_depth + offset);
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

    if(!IS_ROOT(stree, vertexp)) {

        update_chain(stree, vertexp, &chainend, &distance);
        head = get_head(stree, vertexp, &chainend, distance);

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

            vertexp = INNER(stree, rootchild);

            update_chain(stree, vertexp, &chainend, &distance);
            head = get_head(stree, vertexp, &chainend, distance);

            label   = LABEL_START(stree, head);

        } else {

            Wchar labelchar;
            Uint vertex = CHILD(vertexp);

            while(True) {

                if (IS_NOTHING(vertex)) {

                    return patt.start;

                } else if (IS_LEAF(vertex)) {

                    leafnum = LEAF_NUMBER(vertex);
                    label   = LABEL_START(stree, depth + leafnum);

                    if(IS_LAST(label)) {
                        return patt.start;
                    }

                    labelchar = *label;
                    if(labelchar > firstchar) {
                        return patt.start;
                    }

                    if(labelchar == firstchar) {

                        plen = prefixlen(label, &patt, remain);
                        if(MATCHED(plen, patt.end, patt.start)) {
                            return NULL;
                        } else {
                            return patt.start + plen;
                        }
                    }

                    vertex = LEAF_VERTEX(stree, leafnum);

                } else {

                    vertexp  = INNER(stree, vertex);

                    update_chain(stree, vertexp, &chainend, &distance);
                    head = get_head(stree, vertexp, &chainend, distance);

                    label    = LABEL_START(stree, depth + head);
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
void walk(STree *stree)
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

    if(IS_HEADDEPTH_ZERO) {

        // There is no sentinel
        if(IS_SENTINEL(stree->tailptr)) {
            stree->head_end = NULL;
            return;
        }

        firstchar = *(stree->tailptr);
        scanprobe_val = ROOT_CHILD(firstchar);
        if(scanprobe_val == UNDEF) {
            stree->head_end = NULL;
            return;
        }

        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(scanprobe_val)) {

            Pattern edgepatt = make_patt(text + LEAF_NUMBER(scanprobe_val) + 1, sentinel - 1);
            Pattern tailpatt = make_patt(stree->tailptr + 1, sentinel - 1);
            plen = 1 + lcp(edgepatt, tailpatt);
            (stree->tailptr) += plen;
            stree->head_start  = edgepatt.start - 1;
            stree->head_end    = edgepatt.start - 1 + (plen-1);
            stree->split_vertex = scanprobe_val;

            return;
        }

        scanprobe = stree->inner.first + LEAF_NUMBER(scanprobe_val);

        update_chain(stree, scanprobe, &chainend, &distance);
        head = get_head(stree, scanprobe, &chainend, distance);
        depth = get_depth(stree, scanprobe, distance, &chainend);

        label_start = text + head;
        plen = tail_prefixlen(stree, label_start + 1, label_start + depth - 1);

        (stree->tailptr)+= plen;
        if(depth > plen) {
            // cannot reach the successor, fall out of tree
            stree->split_vertex = scanprobe_val;
            stree->head_start    = label_start;
            stree->head_end      = label_start + (plen - 1);
            return;
        }
        stree->headnode = scanprobe;
        stree->head_depth = depth;
    }

    // Head is not the root
    while(True) {
        prev = UNDEF;
        scanprobe_val = CHILD(stree->headnode);
        if(IS_SENTINEL(stree->tailptr)) {
            find_last_successor(stree, &prev, &scanprobe_val);
            stree->split_vertex = NOTHING;
            stree->insertprev   = prev;
            stree->head_end      = NULL;
            return;
        }
        firstchar = *(stree->tailptr);

        do {
            // find successor edge with firstchar = firstchar
            if(IS_LEAF(scanprobe_val)) {

                leafindex = LEAF_NUMBER(scanprobe_val);
                labelchar = get_label(stree, leafindex, &label_start);

                if(labelchar >= firstchar) break;

                prev          = scanprobe_val;
                scanprobe_val = LEAF_SIBLING(stree->leaves.first + leafindex);

            } else {

                scanprobe   = stree->inner.first + LEAF_NUMBER(scanprobe_val);
                update_chain(stree, scanprobe, &chainend, &distance);

                head      = get_head(stree, scanprobe, &chainend, distance);
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
            stree->head_end = NULL;
            return;
        }

        if(IS_LEAF(scanprobe_val)) {
            plen = tail_prefixlen(stree, label_start + 1, sentinel - 1);
            (stree->tailptr) += plen;
            update_stree(stree, label_start, plen, scanprobe_val, prev);
            return;
        }

        depth   = get_depth(stree, scanprobe, distance, &chainend);
        edgelen = depth - stree->head_depth;
        plen    = tail_prefixlen(stree, label_start + 1, label_start + edgelen - 1);
        (stree->tailptr) += plen;

        // cannot reach next node
        if(edgelen > plen) {
            update_stree(stree, label_start, plen, scanprobe_val, prev);
            return;
        }

        stree->headnode = scanprobe;
        stree->head_depth = depth;
    }
}

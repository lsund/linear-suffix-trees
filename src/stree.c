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
Uint textlen;


static bool no_space(STree *stree)
{
    return stree->inner.next >= stree->alloc_leftbound;
}


static Uint *push_leftbound(STree *stree)
{
    return stree->inner.first + stree->inner.size - LARGE_WIDTH;
}

// Allocate space for branch vertices
static void allocate_inner_vertices(STree *stree)
{
    if(no_space(stree)) {

        stree->inner.size += EXTRA_ALLOCSIZE;

        Uint head = INDEX(stree, stree->headnode);
        Uint tmpchainstart = 0;

        if(stree->chainstart != NULL) {
            tmpchainstart = INDEX(stree,stree->chainstart);
        }

        Uint size = stree->inner.size;
        stree->inner.first = ALLOC(stree->inner.first, Uint, size);
        stree->inner.next = stree->inner.first + stree->inner.next_num;
        stree->headnode = stree->inner.first + head;

        if(stree->chainstart != NULL) {
            stree->chainstart = stree->inner.first + tmpchainstart;
        }
        stree->alloc_leftbound = push_leftbound(stree);
    }
}


void insertleaf(STree *stree)
{
    Uint *ptr, newleaf;

    newleaf = MAKE_LEAF(stree->leaf_vertices.next_num);
    if(stree->head_depth == 0)                // head is the root
    {
        if(stree->tailptr != sentinel)      // no \$-edge initially
        {
            stree->rootchildren[(Uint) *(stree->tailptr)] = newleaf;
            *(stree->leaf_vertices.next) = 0;
        }
    } else
    {
        if (stree->insertprev == UNDEF)  // newleaf = first child
        {
            *(stree->leaf_vertices.next) = CHILD(stree->headnode);
            SET_CHILD(stree->headnode,newleaf);
        } else
        {
            if(IS_LEAF(stree->insertprev))   // previous node is leaf
            {
                ptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertprev);
                *(stree->leaf_vertices.next) = *ptr;
                SET_LEAF_SIBLING(ptr,newleaf);
            } else   // previous node is branching node
            {
                ptr = stree->inner.first + LEAF_NUM(stree->insertprev);
                *(stree->leaf_vertices.next) = SIBLING(ptr);
                SET_SIBLING(ptr,newleaf);
            }
        }
    }
stree->leaf_vertices.next_num++;
stree->leaf_vertices.next++;
}

void insertbranchnode(STree *stree)
{
    Uint *ptr, *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(stree->head_depth == 0)      // head is the root
    {

        stree->rootchildren[(Uint) *(stree->headstart)]
            = stree->inner.next_num;
        *(stree->inner.next + 1) = 0;
    } else
    {
        if(stree->insertprev == UNDEF)  // new branch = first child
        {
            SET_CHILD(stree->headnode,stree->inner.next_num);
        } else
        {
            if(IS_LEAF(stree->insertprev))  // new branch = right brother of leaf
            {
                ptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertprev);
                SET_LEAF_SIBLING(ptr,stree->inner.next_num);
            } else                     // new branch = brother of branching node
            {
                SET_SIBLING(stree->inner.first + LEAF_NUM(stree->insertprev),
                        stree->inner.next_num);
            }
        }
    }
    if(IS_LEAF(stree->insertnode))   // split edge is leaf edge
    {
        insertleafptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertnode);
        if (stree->tailptr == sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            // first child =oldleaf
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->insertnode), *insertleafptr);
            // Recall new leaf address
            stree->setlink = stree->leaf_vertices.next;
            stree->setatnewleaf = True;
            SET_LEAF_SIBLING(insertleafptr,                     // new leaf =
                    MAKE_LEAF(stree->leaf_vertices.next_num)); // right brother of old leaf
        } else
        {
            // First child = new leaf
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(stree->leaf_vertices.next_num), *insertleafptr);
            *(stree->leaf_vertices.next) = stree->insertnode;  // old leaf = right brother of of new leaf
            // Recall leaf address
            stree->setlink = insertleafptr;
            stree->setatnewleaf = False;
        }
    } else  // split edge leads to branching node
    {
        insertnodeptr = stree->inner.first + LEAF_NUM(stree->insertnode);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tailptr == sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            // First child is new branch
            // inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE(stree->insertnode), insertnodeptrbrother);
            // Recall new leaf address
            stree->setlink = stree->leaf_vertices.next;
            stree->setatnewleaf = True;
            SET_SIBLING(insertnodeptr,MAKE_LEAF(stree->leaf_vertices.next_num)); // new leaf = brother of old branch
        } else
        {
            // First child is new leaf
            // Inherit brother
            SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(stree->leaf_vertices.next_num), insertnodeptrbrother);
            *(stree->leaf_vertices.next) = stree->insertnode;   // new branch is brother of new leaf
            stree->setlink = insertnodeptr + 1;
            stree->setatnewleaf = False;
        }
    }
    *(stree->setlink) = NOTHING;
    stree->currentdepth = stree->head_depth + (Uint) (stree->headend - stree->headstart+1);
    SET_DEPTH(stree->currentdepth);
    SET_HEAD(stree->leaf_vertices.next_num);
    if (stree->currentdepth > stree->maxbranchdepth) {
        stree->maxbranchdepth = stree->currentdepth;
    }
    stree->leaf_vertices.next_num++;
    stree->leaf_vertices.next++;
}

// Finding the Head-Locs

//
// The function \emph{rescan} finds the location of the current head.
// In order to scan down the tree, it suffices to look at the first
// character of each edge.
//

void rescan(STree *stree) // skip-count
{
    Uint *nodeptr, *largep = NULL, distance = 0, node, prevnode,
         nodedepth, edgelen, wlen, leafindex, head;
    Wchar headchar, edgechar;

    if(stree->head_depth == 0)   // head is the root
    {
        headchar = *(stree->headstart);  // headstart is assumed to be not empty
        node = stree->rootchildren[(Uint) headchar];
        if(IS_LEAF(node))   // stop if successor is leaf
        {
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner.first + LEAF_NUM(node);
        nodedepth = get_depth(stree, nodeptr, &distance, &largep);
        wlen = (Uint) (stree->headend - stree->headstart + 1);
        if(nodedepth > wlen)    // cannot reach the successor node
        {
            stree->insertnode = node;
            return;
        }
        stree->headnode = nodeptr;        // go to successor node
        stree->head_depth = nodedepth;
        if(nodedepth == wlen)             // location has been scanned
        {
            stree->headend = NULL;
            return;
        }
        (stree->headstart) += nodedepth;
    }
    while(True)   // \emph{headnode} is not the root
    {
        headchar = *(stree->headstart);  // \emph{headstart} is assumed to be nonempty
        prevnode = UNDEF;
        node = CHILD(stree->headnode);
        while(True)             // traverse the list of successors
        {
            if(IS_LEAF(node))   // successor is leaf
            {
                leafindex = LEAF_NUM(node);
                edgechar = text[stree->head_depth + leafindex];
                if(edgechar == headchar)    // correct edge found
                {
                    stree->insertnode = node;
                    stree->insertprev = prevnode;
                    return;
                }
                prevnode = node;
                node = stree->leaf_vertices.first[leafindex];
            } else   // successor is branch node
            {
                nodeptr = stree->inner.first + LEAF_NUM(node);
                head = get_head(stree, nodeptr, &largep, &distance);
                edgechar = text[stree->head_depth + head];
                // Correct edge found
                if(edgechar == headchar) {
                    break;
                }
                prevnode = node;
                node = SIBLING(nodeptr);
            }
        }

        nodedepth = get_depth_after_head(stree, nodeptr, &distance, &largep);
        edgelen = nodedepth - stree->head_depth;
        wlen = (Uint) (stree->headend - stree->headstart + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        // go to the successor node
        stree->headnode = nodeptr;
        stree->head_depth = nodedepth;
        if(edgelen == wlen) {
            // location is found
            stree->headend = NULL;
            return;
        }
        (stree->headstart) += edgelen;
    }
}

/*
   The function \emph{taillcp} computes the length of the longest common prefix
   of two strings. The first string is between pointers \emph{start1} and
   \emph{end1}. The second string is the current tail, which is between  the
   pointers \emph{tailptr} and \emph{sentinel}.
   */

static Uint taillcp(STree *stree,Wchar *start1, Wchar *end1)
{
    Wchar *ptr1 = start1, *ptr2 = stree->tailptr + 1;
    while(ptr1 <= end1 && ptr2 < sentinel && *ptr1 == *ptr2)
    {
        ptr1++;
        ptr2++;
    }
    return (Uint) (ptr1-start1);
}

// Scans a prefix of the current tail down from a given node
void scanprefix(STree *stree)
{
    Uint *nodeptr = NULL, *largep = NULL, leafindex, nodedepth, edgelen, node,
         distance = 0, prevnode, prefixlen, head;
    Wchar *leftborder = (Wchar *) NULL, tailchar, edgechar = 0;

    if(stree->head_depth == 0)   // headnode is root
    {

        // there is no \$-edge
        if(stree->tailptr == sentinel) {
            stree->headend = NULL;
            return;
        }
        tailchar = *(stree->tailptr);
        if((node = stree->rootchildren[(Uint) tailchar]) == UNDEF)
        {
            stree->headend = NULL;
            return;
        }
        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(node)) {
            leftborder = text + LEAF_NUM(node);
            prefixlen = 1 + taillcp(stree,leftborder+1,sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner.first + LEAF_NUM(node);
        get_depth_head(stree, &nodedepth, &head, nodeptr, largep);
        leftborder = text + head;
        prefixlen = 1 + taillcp(stree,leftborder+1,leftborder + nodedepth - 1);
        (stree->tailptr)+= prefixlen;
        if(nodedepth > prefixlen)   // cannot reach the successor, fall out of tree
        {
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen - 1);
            stree->insertnode = node;
            return;
        }
        stree->headnode = nodeptr;
        stree->head_depth = nodedepth;
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
                    node = stree->leaf_vertices.first[LEAF_NUM(node)];
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
        tailchar = *(stree->tailptr);

        do // find successor edge with firstchar = tailchar
        {
            if(IS_LEAF(node))   // successor is leaf
            {
                leafindex = LEAF_NUM(node);
                leftborder = text + (stree->head_depth + leafindex);
                if((edgechar = *leftborder) >= tailchar)   // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = stree->leaf_vertices.first[leafindex];
            } else  // successor is branch node
            {
                nodeptr = stree->inner.first + LEAF_NUM(node);
                head = get_head(stree, nodeptr, &largep, &distance);
                leftborder = text + (stree->head_depth + head);
                if((edgechar = *leftborder) >= tailchar)  // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = SIBLING(nodeptr);
            }
        } while(!IS_NOTHING(node));
        if(IS_NOTHING(node) || edgechar > tailchar)  // edge not found
        {
            stree->insertprev = prevnode;   // new edge will become brother of this
            stree->headend = NULL;
            return;
        }
        if(IS_LEAF(node))  // correct edge is leaf edge, compare its label with tail
        {
            prefixlen = 1 + taillcp(stree,leftborder+1,sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        nodedepth = get_depth_after_head(stree, nodeptr, &distance, &largep);
        edgelen = nodedepth - stree->head_depth;
        prefixlen = 1 + taillcp(stree,leftborder+1,leftborder + edgelen - 1);
        (stree->tailptr) += prefixlen;
        if(edgelen > prefixlen)  // cannot reach next node
        {
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        stree->headnode = nodeptr;
        stree->head_depth = nodedepth;
    }
}

void completelarge(STree *stree)
{
    Uint distance, *backwards;

    if(stree->chain_remain > 0)
    {
        backwards = stree->inner.next;
        for(distance = 1; distance <= stree->chain_remain; distance++)
        {
            backwards -= SMALL_WIDTH;
            SET_DISTANCE(backwards,distance);
        }
        stree->chain_remain = 0;
        stree->chainstart = NULL;
    }
    stree->inner.next += LARGE_WIDTH;
    stree->inner.next_num += LARGE_WIDTH;
    stree->n_large++;
}

void linkrootchildren(STree *stree)
{
    Uint *rcptr, *prevnodeptr, prev = UNDEF;

    stree->alphasize = 0;
    for(rcptr = stree->rootchildren;
            rcptr <= stree->rootchildren + MAX_CHARS; rcptr++)
    {
        if(*rcptr != UNDEF)
        {
            stree->alphasize++;
            if(prev == UNDEF)
            {
                SET_CHILD(stree->inner.first, MAKE_LARGE(*rcptr));
            } else
            {
                if(IS_LEAF(prev))
                {
                    stree->leaf_vertices.first[LEAF_NUM(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->inner.first + LEAF_NUM(prev);
                    SET_SIBLING(prevnodeptr,*rcptr);
                }
            }
            prev = *rcptr;
        }
    }
    if(IS_LEAF(prev))
    {
        stree->leaf_vertices.first[LEAF_NUM(prev)] = MAKE_LEAF(textlen);
    } else
    {
        prevnodeptr = stree->inner.first + LEAF_NUM(prev);
        SET_SIBLING(prevnodeptr,MAKE_LEAF(textlen));
    }
    stree->leaf_vertices.first[textlen] = NOTHING;
}


void init(STree *stree)
{
    Uint i;

    stree->inner.size = START_ALLOCSIZE;
    stree->leaf_vertices.first = ALLOC(NULL, Uint, textlen + 2);

    stree->inner.first = ALLOC(NULL, Uint, stree->inner.size);
    for(i=0; i<LARGE_WIDTH; i++) {
        stree->inner.first[i] = 0;
    }

    stree->rootchildren = ALLOC(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEF;
    }

    stree->tailptr = text;
    stree->alloc_leftbound
        = stree->inner.first + stree->inner.size - LARGE_WIDTH;
    stree->headnode = stree->inner.next = stree->inner.first;
    stree->headend = NULL;
    stree->head_depth = stree->maxbranchdepth = 0;

    stree->inner.next = stree->inner.first;
    stree->inner.next_num = 0;

    SET_DEPTH(0);
    SET_HEAD(0);
    SET_CHILD_AND_SIBLING(stree->inner.next, MAKE_LARGE_LEAF(0), 0);
    stree->rootchildren[(Uint) *text] = MAKE_LEAF(0); // Necessary?
    stree->leaf_vertices.first[0]                 = 0;

    stree->leafcounts                   = NULL;
    stree->leaf_vertices.next_num  = 1;
    stree->leaf_vertices.next      = stree->leaf_vertices.first + 1;
    stree->inner.next     = stree->inner.first + LARGE_WIDTH;
    stree->inner.next_num = LARGE_WIDTH;
    stree->insertnode                   = UNDEF;
    stree->insertprev                   = UNDEF;
    stree->chain_remain            = 0;
    stree->chainstart                   = NULL;
    stree->n_large                    = stree->n_small = 0;


}

void freestree(STree *stree)
{
    FREE(stree->leaf_vertices.first);
    FREE(stree->rootchildren);
    FREE(stree->inner.first);
    if(stree->nonmaximal != NULL) {
        FREE(stree->nonmaximal);
    }
    if(stree->leafcounts != NULL) {
        FREE(stree->leafcounts);
    }
}



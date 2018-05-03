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
Uint textlen;


static bool no_space(STree *stree)
{
    return stree->inner_vertices.next_free >= stree->alloc_leftbound;
}


static Uint *push_leftbound(STree *stree)
{
    return stree->inner_vertices.first + stree->inner_vertices.size - LARGEINTS;
}

// Allocate space for branch vertices
static void allocate_inner_vertices(STree *stree)
{
    if(no_space(stree)) {

        stree->inner_vertices.size += EXTRA_ALLOCSIZE;

        Uint head = INDEX_INNER(stree, stree->headnode);
        Uint tmpchainstart = 0;

        if(stree->chainstart != NULL) {
            tmpchainstart = INDEX_INNER(stree,stree->chainstart);
        }

        Uint size = stree->inner_vertices.size;
        stree->inner_vertices.first = ALLOC(stree->inner_vertices.first, Uint, size);
        stree->inner_vertices.next_free = stree->inner_vertices.first + stree->inner_vertices.next_free_num;
        stree->headnode = stree->inner_vertices.first + head;

        if(stree->chainstart != NULL) {
            stree->chainstart = stree->inner_vertices.first + tmpchainstart;
        }
        stree->alloc_leftbound = push_leftbound(stree);
    }
}


void insertleaf(STree *stree)
{
    Uint *ptr, newleaf;

    newleaf = MAKELEAF(stree->leaf_vertices.next_free_num);
    if(stree->headnodedepth == 0)                // head is the root
    {
        if(stree->tailptr != stree->sentinel)      // no \$-edge initially
        {
            stree->rootchildren[(Uint) *(stree->tailptr)] = newleaf;
            *(stree->leaf_vertices.next_free) = 0;
        }
    } else
    {
        if (stree->insertprev == UNDEFREFERENCE)  // newleaf = first child
        {
            *(stree->leaf_vertices.next_free) = CHILD(stree->headnode);
            SETCHILD(stree->headnode,newleaf);
        } else
        {
            if(IS_LEAF(stree->insertprev))   // previous node is leaf
            {
                ptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertprev);
                *(stree->leaf_vertices.next_free) = *ptr;
                SETLEAFBROTHER(ptr,newleaf);
            } else   // previous node is branching node
            {
                ptr = stree->inner_vertices.first + LEAF_NUM(stree->insertprev);
                *(stree->leaf_vertices.next_free) = SIBLING(ptr);
                SETBROTHER(ptr,newleaf);
            }
        }
    }
stree->leaf_vertices.next_free_num++;
stree->leaf_vertices.next_free++;
}

void insertbranchnode(STree *stree)
{
    Uint *ptr, *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    allocate_inner_vertices(stree);
    if(stree->headnodedepth == 0)      // head is the root
    {

        stree->rootchildren[(Uint) *(stree->headstart)]
            = MAKEBRANCHADDR(stree->inner_vertices.next_free_num);
        *(stree->inner_vertices.next_free + 1) = 0;
    } else
    {
        if(stree->insertprev == UNDEFREFERENCE)  // new branch = first child
        {
            SETCHILD(stree->headnode,MAKEBRANCHADDR(stree->inner_vertices.next_free_num));
        } else
        {
            if(IS_LEAF(stree->insertprev))  // new branch = right brother of leaf
            {
                ptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertprev);
                SETLEAFBROTHER(ptr,MAKEBRANCHADDR(stree->inner_vertices.next_free_num));
            } else                     // new branch = brother of branching node
            {
                SETBROTHER(stree->inner_vertices.first + LEAF_NUM(stree->insertprev),
                        MAKEBRANCHADDR(stree->inner_vertices.next_free_num));
            }
        }
    }
    if(IS_LEAF(stree->insertnode))   // split edge is leaf edge
    {
        insertleafptr = stree->leaf_vertices.first + LEAF_NUM(stree->insertnode);
        if (stree->tailptr == stree->sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            SETNEWCHILDBROTHER(MAKELARGE(stree->insertnode),  // first child=oldleaf
                    *insertleafptr);  // inherit brother
            RECALLNEWLEAFADDRESS(stree->leaf_vertices.next_free);
            SETLEAFBROTHER(insertleafptr,                     // new leaf =
                    MAKELEAF(stree->leaf_vertices.next_free_num)); // right brother of old leaf
        } else
        {
            SETNEWCHILDBROTHER(MAKELARGELEAF(stree->leaf_vertices.next_free_num),  // first child=new leaf
                    *insertleafptr);  // inherit brother
            *(stree->leaf_vertices.next_free) = stree->insertnode;  // old leaf = right brother of of new leaf
            RECALLLEAFADDRESS(insertleafptr);
        }
    } else  // split edge leads to branching node
    {
        insertnodeptr = stree->inner_vertices.first + LEAF_NUM(stree->insertnode);
        insertnodeptrbrother = SIBLING(insertnodeptr);
        if (stree->tailptr == stree->sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            SETNEWCHILDBROTHER(MAKELARGE(stree->insertnode), // first child new branch
                    insertnodeptrbrother);        // inherit right brother
            RECALLNEWLEAFADDRESS(stree->leaf_vertices.next_free);
            SETBROTHER(insertnodeptr,MAKELEAF(stree->leaf_vertices.next_free_num)); // new leaf = brother of old branch
        } else
        {
            SETNEWCHILDBROTHER(MAKELARGELEAF(stree->leaf_vertices.next_free_num), // first child is new leaf
                    insertnodeptrbrother);        // inherit brother
            *(stree->leaf_vertices.next_free) = stree->insertnode;   // new branch is brother of new leaf
            stree->setlink = insertnodeptr + 1;
            stree->setatnewleaf = False;
        }
    }
    *(stree->setlink) = NILBIT;
    stree->currentdepth = stree->headnodedepth + (Uint) (stree->headend-stree->headstart+1);
    SETDEPTHHEADPOS(stree->currentdepth,stree->leaf_vertices.next_free_num);
    if (stree->currentdepth > stree->maxbranchdepth) {
        stree->maxbranchdepth = stree->currentdepth;
    }
    stree->leaf_vertices.next_free_num++;
    stree->leaf_vertices.next_free++;
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

    if(stree->headnodedepth == 0)   // head is the root
    {
        headchar = *(stree->headstart);  // headstart is assumed to be not empty
        node = stree->rootchildren[(Uint) headchar];
        if(IS_LEAF(node))   // stop if successor is leaf
        {
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner_vertices.first + LEAF_NUM(node);
        nodedepth = get_depth(stree, nodeptr, &distance, &largep);
        wlen = (Uint) (stree->headend - stree->headstart + 1);
        if(nodedepth > wlen)    // cannot reach the successor node
        {
            stree->insertnode = node;
            return;
        }
        stree->headnode = nodeptr;        // go to successor node
        stree->headnodedepth = nodedepth;
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
        prevnode = UNDEFREFERENCE;
        node = CHILD(stree->headnode);
        while(True)             // traverse the list of successors
        {
            if(IS_LEAF(node))   // successor is leaf
            {
                leafindex = LEAF_NUM(node);
                edgechar = stree->text[stree->headnodedepth + leafindex];
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
                nodeptr = stree->inner_vertices.first + LEAF_NUM(node);
                head = get_head(stree, nodeptr, &largep, &distance);
                edgechar = stree->text[stree->headnodedepth + head];
                // Correct edge found
                if(edgechar == headchar) {
                    break;
                }
                prevnode = node;
                node = SIBLING(nodeptr);
            }
        }

        nodedepth = get_depth_after_head(stree, nodeptr, &distance, &largep);
        edgelen = nodedepth - stree->headnodedepth;
        wlen = (Uint) (stree->headend - stree->headstart + 1);
        if(edgelen > wlen) {
            // cannot reach the succ node
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        // go to the successor node
        stree->headnode = nodeptr;
        stree->headnodedepth = nodedepth;
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
    while(ptr1 <= end1 && ptr2 < stree->sentinel && *ptr1 == *ptr2)
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

    if(stree->headnodedepth == 0)   // headnode is root
    {

        // there is no \$-edge
        if(stree->tailptr == stree->sentinel) {
            stree->headend = NULL;
            return;
        }
        tailchar = *(stree->tailptr);
        if((node = stree->rootchildren[(Uint) tailchar]) == UNDEFREFERENCE)
        {
            stree->headend = NULL;
            return;
        }
        // successor edge is leaf, compare tail and leaf edge label
        if(IS_LEAF(node)) {
            leftborder = stree->text + LEAF_NUM(node);
            prefixlen = 1 + taillcp(stree,leftborder+1,stree->sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner_vertices.first + LEAF_NUM(node);
        get_depth_head(stree, &nodedepth, &head, nodeptr, largep);
        leftborder = stree->text + head;
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
        stree->headnodedepth = nodedepth;
    }
    while(True)  // \emph{headnode} is not the root
    {
        prevnode = UNDEFREFERENCE;
        node = CHILD(stree->headnode);
        if(stree->tailptr == stree->sentinel)  //  \$-edge
        {
            do // there is no \$-edge, so find last successor, of which it becomes right brother
            {
                prevnode = node;
                if(IS_LEAF(node))
                {
                    node = stree->leaf_vertices.first[LEAF_NUM(node)];
                } else
                {
                    node = SIBLING(stree->inner_vertices.first + LEAF_NUM(node));
                }
            } while(!IS_NOTHING(node));
            stree->insertnode = NILBIT;
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
                leftborder = stree->text + (stree->headnodedepth + leafindex);
                if((edgechar = *leftborder) >= tailchar)   // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = stree->leaf_vertices.first[leafindex];
            } else  // successor is branch node
            {
                nodeptr = stree->inner_vertices.first + LEAF_NUM(node);
                head = get_head(stree, nodeptr, &largep, &distance);
                leftborder = stree->text + (stree->headnodedepth + head);
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
            prefixlen = 1 + taillcp(stree,leftborder+1,stree->sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        nodedepth = get_depth_after_head(stree, nodeptr, &distance, &largep);
        edgelen = nodedepth - stree->headnodedepth;
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
        stree->headnodedepth = nodedepth;
    }
}

void completelarge(STree *stree)
{
    Uint distance, *backwards;

    if(stree->smallnotcompleted > 0)
    {
        backwards = stree->inner_vertices.next_free;
        for(distance = 1; distance <= stree->smallnotcompleted; distance++)
        {
            backwards -= SMALLINTS;
            SETDISTANCE(backwards,distance);
        }
        stree->smallnotcompleted = 0;
        stree->chainstart = NULL;
    }
    stree->inner_vertices.next_free += LARGEINTS;
    stree->inner_vertices.next_free_num += LARGEINTS;
    stree->largenode++;
}

void linkrootchildren(STree *stree)
{
    Uint *rcptr, *prevnodeptr, prev = UNDEFREFERENCE;

    stree->alphasize = 0;
    for(rcptr = stree->rootchildren;
            rcptr <= stree->rootchildren + MAX_CHARS; rcptr++)
    {
        if(*rcptr != UNDEFREFERENCE)
        {
            stree->alphasize++;
            if(prev == UNDEFREFERENCE)
            {
                SETCHILD(stree->inner_vertices.first, MAKELARGE(*rcptr));
            } else
            {
                if(IS_LEAF(prev))
                {
                    stree->leaf_vertices.first[LEAF_NUM(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->inner_vertices.first + LEAF_NUM(prev);
                    SETBROTHER(prevnodeptr,*rcptr);
                }
            }
            prev = *rcptr;
        }
    }
    if(IS_LEAF(prev))
    {
        stree->leaf_vertices.first[LEAF_NUM(prev)] = MAKELEAF(stree->textlen);
    } else
    {
        prevnodeptr = stree->inner_vertices.first + LEAF_NUM(prev);
        SETBROTHER(prevnodeptr,MAKELEAF(stree->textlen));
    }
    stree->leaf_vertices.first[stree->textlen] = NILBIT;
}


void init(STree *stree)
{
    Uint i;

    stree->inner_vertices.size = START_ALLOCSIZE;
    stree->leaf_vertices.first = ALLOC(NULL, Uint, textlen + 2);

    stree->inner_vertices.first = ALLOC(NULL, Uint, stree->inner_vertices.size);
    for(i=0; i<LARGEINTS; i++) {
        stree->inner_vertices.first[i] = 0;
    }

    stree->rootchildren = ALLOC(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEFREFERENCE;
    }

    stree->text = text;
    stree->tailptr = text;
    stree->textlen = textlen;
    stree->sentinel = text + textlen;
    stree->alloc_leftbound
        = stree->inner_vertices.first + stree->inner_vertices.size - LARGEINTS;
    stree->headnode = stree->inner_vertices.next_free = stree->inner_vertices.first;
    stree->headend = NULL;
    stree->headnodedepth = stree->maxbranchdepth = 0;

    stree->inner_vertices.next_free = stree->inner_vertices.first;
    stree->inner_vertices.next_free_num = 0;

    SETDEPTHHEADPOS(0, 0);
    SETNEWCHILDBROTHER(MAKELARGELEAF(0),0);
    SETBRANCHNODEOFFSET;
    stree->rootchildren[(Uint) *text] = MAKELEAF(0); // Necessary?
    stree->leaf_vertices.first[0]                 = 0;

    stree->leafcounts                   = NULL;
    stree->leaf_vertices.next_free_num  = 1;
    stree->leaf_vertices.next_free      = stree->leaf_vertices.first + 1;
    stree->inner_vertices.next_free     = stree->inner_vertices.first + LARGEINTS;
    stree->inner_vertices.next_free_num = LARGEINTS;
    stree->insertnode                   = UNDEFREFERENCE;
    stree->insertprev                   = UNDEFREFERENCE;
    stree->smallnotcompleted            = 0;
    stree->chainstart                   = NULL;
    stree->largenode                    = stree->smallnode = 0;


}

void freestree(STree *stree)
{
    FREE(stree->leaf_vertices.first);
    FREE(stree->rootchildren);
    FREE(stree->inner_vertices.first);
    if(stree->nonmaximal != NULL)
    {
        FREE(stree->nonmaximal);
    }
    if(stree->leafcounts != NULL)
    {
        FREE(stree->leafcounts);
    }
}



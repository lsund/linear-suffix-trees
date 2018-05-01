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


static Uint max(Uint a, Uint b)
{
    return a > b ? a : b;
}


static Uint start_alloc(Uint textlen)
{
    return max(0.5 * MULTBYSMALLINTS(textlen+1), 48);
}


static Uint extra_alloc(Uint textlen)
{
    return max(0.05 * MULTBYSMALLINTS(textlen + 1), 48);
}


static bool no_space(Suffixtree *stree)
{
    return stree->inner_vertices.next_free >= stree->alloc_leftbound;
}


static Uint *push_leftbound(Suffixtree *stree)
{
    return stree->inner_vertices.first + stree->inner_vertices.size - LARGEINTS;
}

// Allocate space for branch vertices
static void allocate_inner_vertices(Suffixtree *stree)
{
    if(no_space(stree)) {

        Uint extra = extra_alloc(stree->textlen);
        stree->inner_vertices.size += extra;

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


void insertleaf(Suffixtree *stree)
{
    Uint *ptr, newleaf;

    newleaf = MAKELEAF(stree->nextfreeleafnum);
    if(stree->headnodedepth == 0)                // head is the root
    {
        if(stree->tailptr != stree->sentinel)      // no \$-edge initially
        {
            stree->rootchildren[(Uint) *(stree->tailptr)] = newleaf;
            *(stree->nextfreeleafptr) = 0;
        }
    } else
    {
        if (stree->insertprev == UNDEFREFERENCE)  // newleaf = first child
        {
            *(stree->nextfreeleafptr) = GETCHILD(stree->headnode);
            SETCHILD(stree->headnode,newleaf);
        } else
        {
            if(ISLEAF(stree->insertprev))   // previous node is leaf
            {
                ptr = stree->leaftab + GETLEAFINDEX(stree->insertprev);
                *(stree->nextfreeleafptr) = LEAFBROTHERVAL(*ptr);
                SETLEAFBROTHER(ptr,newleaf);
            } else   // previous node is branching node
            {
                ptr = stree->inner_vertices.first + GETBRANCHINDEX(stree->insertprev);
                *(stree->nextfreeleafptr) = GETBROTHER(ptr);
                SETBROTHER(ptr,newleaf);
            }
        }
    }
    RECALLSUCC(newleaf);     // recall node on successor path of \emph{headnode}
stree->nextfreeleafnum++;
stree->nextfreeleafptr++;
}

void insertbranchnode(Suffixtree *stree)
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
            if(ISLEAF(stree->insertprev))  // new branch = right brother of leaf
            {
                ptr = stree->leaftab + GETLEAFINDEX(stree->insertprev);
                SETLEAFBROTHER(ptr,MAKEBRANCHADDR(stree->inner_vertices.next_free_num));
            } else                     // new branch = brother of branching node
            {
                SETBROTHER(stree->inner_vertices.first + GETBRANCHINDEX(stree->insertprev),
                        MAKEBRANCHADDR(stree->inner_vertices.next_free_num));
            }
        }
    }
    if(ISLEAF(stree->insertnode))   // split edge is leaf edge
    {
        insertleafptr = stree->leaftab + GETLEAFINDEX(stree->insertnode);
        if (stree->tailptr == stree->sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            SETNEWCHILDBROTHER(MAKELARGE(stree->insertnode),  // first child=oldleaf
                    LEAFBROTHERVAL(*insertleafptr));  // inherit brother
            RECALLNEWLEAFADDRESS(stree->nextfreeleafptr);
            SETLEAFBROTHER(insertleafptr,                     // new leaf =
                    MAKELEAF(stree->nextfreeleafnum)); // right brother of old leaf
        } else
        {
            SETNEWCHILDBROTHER(MAKELARGELEAF(stree->nextfreeleafnum),  // first child=new leaf
                    LEAFBROTHERVAL(*insertleafptr));  // inherit brother
            *(stree->nextfreeleafptr) = stree->insertnode;  // old leaf = right brother of of new leaf
            RECALLLEAFADDRESS(insertleafptr);
        }
    } else  // split edge leads to branching node
    {
        insertnodeptr = stree->inner_vertices.first + GETBRANCHINDEX(stree->insertnode);
        insertnodeptrbrother = GETBROTHER(insertnodeptr);
        if (stree->tailptr == stree->sentinel ||
                *(stree->headend+1) < *(stree->tailptr))
        {
            SETNEWCHILDBROTHER(MAKELARGE(stree->insertnode), // first child new branch
                    insertnodeptrbrother);        // inherit right brother
            RECALLNEWLEAFADDRESS(stree->nextfreeleafptr);
            SETBROTHER(insertnodeptr,MAKELEAF(stree->nextfreeleafnum)); // new leaf = brother of old branch
        } else
        {
            SETNEWCHILDBROTHER(MAKELARGELEAF(stree->nextfreeleafnum), // first child is new leaf
                    insertnodeptrbrother);        // inherit brother
            *(stree->nextfreeleafptr) = stree->insertnode;   // new branch is brother of new leaf
            RECALLBRANCHADDRESS(insertnodeptr);
        }
    }
    SETNILBIT;
    RECALLSUCC(MAKEBRANCHADDR(stree->inner_vertices.next_free)); // node on succ. path
    stree->currentdepth = stree->headnodedepth + (Uint) (stree->headend-stree->headstart+1);
    SETDEPTHHEADPOS(stree->currentdepth,stree->nextfreeleafnum);
    SETMAXBRANCHDEPTH(stree->currentdepth);
    stree->nextfreeleafnum++;
    stree->nextfreeleafptr++;
}

// Finding the Head-Locations

//
// The function \emph{rescan} finds the location of the current head.
// In order to scan down the tree, it suffices to look at the first
// character of each edge.
//

void rescan(Suffixtree *stree) // skip-count
{
    Uint *nodeptr, *largeptr = NULL, distance = 0, node, prevnode,
         nodedepth, edgelen, wlen, leafindex, headposition;
    wchar_t headchar, edgechar;

    if(stree->headnodedepth == 0)   // head is the root
    {
        headchar = *(stree->headstart);  // headstart is assumed to be not empty
        node = stree->rootchildren[(Uint) headchar];
        if(ISLEAF(node))   // stop if successor is leaf
        {
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
        GETONLYDEPTH(nodedepth,nodeptr);
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
        node = GETCHILD(stree->headnode);
        while(True)             // traverse the list of successors
        {
            if(ISLEAF(node))   // successor is leaf
            {
                leafindex = GETLEAFINDEX(node);
                edgechar = stree->text[stree->headnodedepth + leafindex];
                if(edgechar == headchar)    // correct edge found
                {
                    stree->insertnode = node;
                    stree->insertprev = prevnode;
                    return;
                }
                prevnode = node;
                node = LEAFBROTHERVAL(stree->leaftab[leafindex]);
            } else   // successor is branch node
            {
                nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
                GETONLYHEADPOS(headposition,nodeptr);
                edgechar = stree->text[stree->headnodedepth + headposition];
                if(edgechar == headchar) // correct edge found
                {
                    break;
                }
                prevnode = node;
                node = GETBROTHER(nodeptr);
            }
        }

        GETDEPTHAFTERHEADPOS(nodedepth,nodeptr);     // get info about succ node
        edgelen = nodedepth - stree->headnodedepth;
        wlen = (Uint) (stree->headend - stree->headstart + 1);
        if(edgelen > wlen)     // cannot reach the succ node
        {
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        stree->headnode = nodeptr;    // go to the successor node
        stree->headnodedepth = nodedepth;
        if(edgelen == wlen)                    // location is found
        {
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

static Uint taillcp(Suffixtree *stree,wchar_t *start1, wchar_t *end1)
{
    wchar_t *ptr1 = start1, *ptr2 = stree->tailptr + 1;
    while(ptr1 <= end1 && ptr2 < stree->sentinel && *ptr1 == *ptr2)
    {
        ptr1++;
        ptr2++;
    }
    return (Uint) (ptr1-start1);
}

// Scans a prefix of the current tail down from a given node
void scanprefix(Suffixtree *stree)
{
    Uint *nodeptr = NULL, *largeptr = NULL, leafindex, nodedepth, edgelen, node,
         distance = 0, prevnode, prefixlen, headposition;
    wchar_t *leftborder = (wchar_t *) NULL, tailchar, edgechar = 0;

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
        if(ISLEAF(node)) {
            leftborder = stree->text + GETLEAFINDEX(node);
            prefixlen = 1 + taillcp(stree,leftborder+1,stree->sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
        GETBOTH(nodedepth,headposition,nodeptr);  // get info for branch node
        leftborder = stree->text + headposition;
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
        node = GETCHILD(stree->headnode);
        if(stree->tailptr == stree->sentinel)  //  \$-edge
        {
            do // there is no \$-edge, so find last successor, of which it becomes right brother
            {
                prevnode = node;
                if(ISLEAF(node))
                {
                    node = LEAFBROTHERVAL(stree->leaftab[GETLEAFINDEX(node)]);
                } else
                {
                    node = GETBROTHER(stree->inner_vertices.first + GETBRANCHINDEX(node));
                }
            } while(!NILPTR(node));
            stree->insertnode = NILBIT;
            stree->insertprev = prevnode;
            stree->headend = NULL;
            return;
        }
        tailchar = *(stree->tailptr);

        do // find successor edge with firstchar = tailchar
        {
            if(ISLEAF(node))   // successor is leaf
            {
                leafindex = GETLEAFINDEX(node);
                leftborder = stree->text + (stree->headnodedepth + leafindex);
                if((edgechar = *leftborder) >= tailchar)   // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = LEAFBROTHERVAL(stree->leaftab[leafindex]);
            } else  // successor is branch node
            {
                nodeptr = stree->inner_vertices.first + GETBRANCHINDEX(node);
                GETONLYHEADPOS(headposition,nodeptr);
                leftborder = stree->text + (stree->headnodedepth + headposition);
                if((edgechar = *leftborder) >= tailchar)  // edge will not come later
                {
                    break;
                }
                prevnode = node;
                node = GETBROTHER(nodeptr);
            }
        } while(!NILPTR(node));
        if(NILPTR(node) || edgechar > tailchar)  // edge not found
        {
            stree->insertprev = prevnode;   // new edge will become brother of this
            stree->headend = NULL;
            return;
        }
        if(ISLEAF(node))  // correct edge is leaf edge, compare its label with tail
        {
            prefixlen = 1 + taillcp(stree,leftborder+1,stree->sentinel-1);
            (stree->tailptr) += prefixlen;
            stree->headstart = leftborder;
            stree->headend = leftborder + (prefixlen-1);
            stree->insertnode = node;
            stree->insertprev = prevnode;
            return;
        }
        GETDEPTHAFTERHEADPOS(nodedepth,nodeptr); // we already know headposition
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

void completelarge(Suffixtree *stree)
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

void linkrootchildren(Suffixtree *stree)
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
                if(ISLEAF(prev))
                {
                    stree->leaftab[GETLEAFINDEX(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->inner_vertices.first + GETBRANCHINDEX(prev);
                    SETBROTHER(prevnodeptr,*rcptr);
                }
            }
            prev = *rcptr;
        }
    }
    if(ISLEAF(prev))
    {
        stree->leaftab[GETLEAFINDEX(prev)] = MAKELEAF(stree->textlen);
    } else
    {
        prevnodeptr = stree->inner_vertices.first + GETBRANCHINDEX(prev);
        SETBROTHER(prevnodeptr,MAKELEAF(stree->textlen));
    }
    stree->leaftab[stree->textlen] = NILBIT;
}


void init(Suffixtree *stree,wchar_t *text,Uint textlen)
{
    Uint i;

    stree->inner_vertices.size = start_alloc(textlen);

    stree->leaftab = ALLOC(NULL, Uint, textlen + 2);

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
    stree->leaftab[0]                 = 0;

    stree->leafcounts                   = NULL;
    stree->nextfreeleafnum              = 1;
    stree->nextfreeleafptr              = stree->leaftab + 1;
    stree->inner_vertices.next_free              = stree->inner_vertices.first + LARGEINTS;
    stree->inner_vertices.next_free_num = LARGEINTS;
    stree->insertnode                   = UNDEFREFERENCE;
    stree->insertprev                   = UNDEFREFERENCE;
    stree->smallnotcompleted            = 0;
    stree->chainstart                   = NULL;
    stree->largenode                    = stree->smallnode                         = 0;


}

void freestree(Suffixtree *stree)
{
    FREE(stree->leaftab);
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


Uint getlargelinkconstruction(Suffixtree *stree)
{
    wchar_t secondchar;

    if(stree->headnodedepth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->headnodedepth == 2)  // determine second char of egde
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    return *(stree->headnode+4);
}

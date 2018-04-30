/*
   Copyright (c) 2003 by Stefan Kurtz and The Institute for
   Genomic Research.  This is OSI Certified Open Source Software.
   Please see the file LICENSE for licensing information and
   the file ACKNOWLEDGEMENTS for names of contributors to the
   code base.
   */

#include "bitvector.h"
#include "spaceman.h"
#include "megabytes.h"
#include "debugdef.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "basedef.h"
/* #include "streesmall.h" */
#include "streelarge.h"

#define FUNCLEVEL 4

#define DEBUGDEFAULT DEBUG1(FUNCLEVEL,">%s\n",__func__);\
    DEBUGCODE(5,showstree(stree))

#define VALIDINIT     0

#define CHECKTEXTLEN\
    if(textlen > MAXTEXTLEN)\
{\
    ERROR2("suffix tree construction failed: "\
            "textlen=%lu larger than maximal textlen=%lu",\
            (Ulong) textlen,(Ulong) MAXTEXTLEN);\
    return -1;\
}

#ifdef DEBUG


static void showvalues(void)
{
    SHOWVAL(SMALLINTS);
    SHOWVAL(LARGEINTS);
    SHOWVAL(MAXDISTANCE);
#if defined(STREELARGE) || defined(STREESMALL)
    SHOWVAL(SMALLDEPTH);
#endif
    SHOWVAL(MAXTEXTLEN);
}

#endif

//}

/*
   This file contains code for the improved linked list implementation,
   as described in \cite{KUR:1998,KUR:BAL:1999}. It can be
   compiled with two options:
   \begin{itemize}
   \item
   for short strings of length \(\leq 2^{21}-1=2\) megabytes,
   we recommend the option
   \texttt{STREESMALL}. This results in a representation which requires
   \(2\) integers for each small node, and three integers for each large node.
   See also the header file \texttt{streesmall.h}.
   \item
   for long strings of length \(\leq 2^{27}-1=12\) megabytes, we
   recommend the option
   \texttt{STREELARGE}. This results in a representation which requires
   \(2\) integers for each small node, and four integers for each large node.
   See also the header file \texttt{streelarge.h}.
   \end{itemize}
   */

// For a string of length \(n\) we initially allocate space for
// \(\texttt{STARTFACTOR}\cdot\texttt{SMALLINTS}\cdot n\) integers to store
// the branching nodes. This usually suffices for most cases. In case we need
// more integers, we allocate space for \(\texttt{ADDFACTOR}\cdot n\)
// (at least 16) extra branching nodes.

#ifndef STARTFACTOR
#define STARTFACTOR 0.5
#endif

#define ADDFACTOR   0.05
#define MINEXTRA    16

// Before a new node is stored, we check if there is enough space available.
// If not, the space is enlarged by a small amount. Since some global pointers
// directly refer into the table, these have to be adjusted after reallocation.
static void spaceforbranchtab(Suffixtree *stree)
{
    DEBUG1(FUNCLEVEL,">%s\n",__func__);

    if(stree->nextfreebranch >= stree->firstnotallocated)
    {
        Uint tmpheadnode,
             tmpchainstart = 0,
             extra = (Uint) (ADDFACTOR * (MULTBYSMALLINTS(stree->textlen+1)));
        if(extra < MINEXTRA)
        {
            extra = MULTBYSMALLINTS(MINEXTRA);
        }
        DEBUG1(2,"#all suffixes up to suffix %lu have been scanned\n",
                (Ulong) stree->nextfreeleafnum);
        DEBUG1(2,"#current space peak %f\n",MEGABYTES(getspacepeak()));
        DEBUG1(2,"#to get %lu extra space do ",(Ulong) extra);
        stree->currentbranchtabsize += extra;
        tmpheadnode = BRADDR2NUM(stree,stree->headnode);
        if(stree->chainstart != NULL)
        {
            tmpchainstart = BRADDR2NUM(stree,stree->chainstart);
        }

        stree->branchtab
            = ALLOCSPACE(stree->branchtab,Uint,stree->currentbranchtabsize);
        stree->nextfreebranch = stree->branchtab + stree->nextfreebranchnum;
        stree->headnode = stree->branchtab + tmpheadnode;
        if(stree->chainstart != NULL)
        {
            stree->chainstart = stree->branchtab + tmpchainstart;
        }
        stree->firstnotallocated
            = stree->branchtab + stree->currentbranchtabsize - LARGEINTS;
    }
}

#ifdef STREEHUGE

static Uint getlargelinkconstruction(Suffixtree *stree)
{
    SYMBOL secondchar;

    DEBUG2(FUNCLEVEL,">%s(%lu)\n",
            __func__,
            (Ulong) BRADDR2NUM(stree,stree->headnode));
    if(stree->headnodedepth == 1)
    {
        return 0;        // link refers to root
    }
    if(stree->headnodedepth == 2)  // determine second char of egde
    {
        if(stree->headend == NULL)
        {
            secondchar = *(stree->tailptr-1);
        } else
        {
            secondchar = *(stree->tailptr - (stree->headend - stree->headstart + 2));
        }
        return stree->rootchildren[(Uint) secondchar];
    }
    return *(stree->headnode+4);
}
#endif


// Insertion of Nodes

//
// The function \emph{insertleaf} inserts a leaf and a corresponding leaf
// edge outgoing from the current \emph{headnode}.
// \emph{insertprev} refers to the node to the left of the leaf to be inserted.
// If the leaf is the first child, then \emph{insertprev} is
// \texttt{UNDEFREFERENCE}.
//

static void insertleaf(Suffixtree *stree)
{
    Uint *ptr, newleaf;

    DEBUGDEFAULT;
    newleaf = MAKELEAF(stree->nextfreeleafnum);
    DEBUGCODE(1,stree->insertleafcalls++);
    if(stree->headnodedepth == 0)                // head is the root
    {
        if(stree->tailptr != stree->sentinel)      // no \$-edge initially
        {
            stree->rootchildren[(Uint) *(stree->tailptr)] = newleaf;
            *(stree->nextfreeleafptr) = VALIDINIT;
            DEBUG2(4,"%c-edge from root points to leaf %lu\n",
                    *(stree->tailptr),(Ulong) stree->nextfreeleafnum);
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
                ptr = stree->branchtab + GETBRANCHINDEX(stree->insertprev);
                *(stree->nextfreeleafptr) = GETBROTHER(ptr);
                SETBROTHER(ptr,newleaf);
            }
        }
    }
    RECALLSUCC(newleaf);     // recall node on successor path of \emph{headnode}
stree->nextfreeleafnum++;
stree->nextfreeleafptr++;
}

//
// The function \emph{insertbranch} inserts a branching node and splits
// the appropriate edges, according to the canonical location of the current
// head. \emph{insertprev} refers to the node to the left of the branching
// node to be inserted. If the branching node is the first child, then
// \emph{insertprev} is \texttt{UNDEFREFERENCE}. The edge to be split ends
// in the node referred to by \emph{insertnode}.
//

static void insertbranchnode(Suffixtree *stree)
{
    Uint *ptr, *insertnodeptr, *insertleafptr, insertnodeptrbrother;

    DEBUGDEFAULT;
    spaceforbranchtab(stree);
    if(stree->headnodedepth == 0)      // head is the root
    {

        stree->rootchildren[(Uint) *(stree->headstart)]
            = MAKEBRANCHADDR(stree->nextfreebranchnum);
        *(stree->nextfreebranch+1) = VALIDINIT;
        DEBUG2(4,"%c-edge from root points to branch node with address %lu\n",
                *(stree->headstart),(Ulong) stree->nextfreebranchnum);
    } else
    {
        if(stree->insertprev == UNDEFREFERENCE)  // new branch = first child
        {
            SETCHILD(stree->headnode,MAKEBRANCHADDR(stree->nextfreebranchnum));
        } else
        {
            if(ISLEAF(stree->insertprev))  // new branch = right brother of leaf
            {
                ptr = stree->leaftab + GETLEAFINDEX(stree->insertprev);
                SETLEAFBROTHER(ptr,MAKEBRANCHADDR(stree->nextfreebranchnum));
            } else                     // new branch = brother of branching node
            {
                SETBROTHER(stree->branchtab + GETBRANCHINDEX(stree->insertprev),
                        MAKEBRANCHADDR(stree->nextfreebranchnum));
            }
        }
    }
    if(ISLEAF(stree->insertnode))   // split edge is leaf edge
    {
        DEBUGCODE(1,stree->splitleafedge++);
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
        DEBUGCODE(1,stree->splitinternaledge++);
        insertnodeptr = stree->branchtab + GETBRANCHINDEX(stree->insertnode);
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
    RECALLSUCC(MAKEBRANCHADDR(stree->nextfreebranchnum)); // node on succ. path
    stree->currentdepth = stree->headnodedepth + (Uint) (stree->headend-stree->headstart+1);
    SETDEPTHHEADPOS(stree->currentdepth,stree->nextfreeleafnum);
    SETMAXBRANCHDEPTH(stree->currentdepth);
    stree->nextfreeleafnum++;
    stree->nextfreeleafptr++;
    DEBUGCODE(1,stree->nodecount++);
}

// Finding the Head-Locations

//
// The function \emph{rescan} finds the location of the current head.
// In order to scan down the tree, it suffices to look at the first
// character of each edge.
//

static void rescan(Suffixtree *stree) // skip-count
{
    Uint *nodeptr, *largeptr = NULL, distance = 0, node, prevnode,
         nodedepth, edgelen, wlen, leafindex, headposition;
    SYMBOL headchar, edgechar;

    DEBUGDEFAULT;
    if(stree->headnodedepth == 0)   // head is the root
    {
        headchar = *(stree->headstart);  // headstart is assumed to be not empty
        node = stree->rootchildren[(Uint) headchar];
        if(ISLEAF(node))   // stop if successor is leaf
        {
            stree->insertnode = node;
            return;
        }
        nodeptr = stree->branchtab + GETBRANCHINDEX(node);
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
                nodeptr = stree->branchtab + GETBRANCHINDEX(node);
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

static Uint taillcp(Suffixtree *stree,SYMBOL *start1, SYMBOL *end1)
{
    SYMBOL *ptr1 = start1, *ptr2 = stree->tailptr + 1;
    while(ptr1 <= end1 && ptr2 < stree->sentinel && *ptr1 == *ptr2)
    {
        ptr1++;
        ptr2++;
    }
    return (Uint) (ptr1-start1);
}

// Scans a prefix of the current tail down from a given node
static void scanprefix(Suffixtree *stree) // probably slow-scan / walk
{
    Uint *nodeptr = NULL, *largeptr = NULL, leafindex, nodedepth, edgelen, node,
         distance = 0, prevnode, prefixlen, headposition;
    SYMBOL *leftborder = (SYMBOL *) NULL, tailchar, edgechar = 0;

    DEBUGDEFAULT;
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
        nodeptr = stree->branchtab + GETBRANCHINDEX(node);
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
                    node = GETBROTHER(stree->branchtab + GETBRANCHINDEX(node));
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
                nodeptr = stree->branchtab + GETBRANCHINDEX(node);
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

//\subsection{Completion and Initialization}

/*
   The function \emph{completelarge} is called whenever a large node
   is inserted. It basically sets the appropriate distance values of the small
   nodes of the current chain.
   */

static void completelarge(Suffixtree *stree)
{
    Uint distance, *backwards;

    DEBUGDEFAULT;
    if(stree->smallnotcompleted > 0)
    {
        backwards = stree->nextfreebranch;
        for(distance = 1; distance <= stree->smallnotcompleted; distance++)
        {
            backwards -= SMALLINTS;
            SETDISTANCE(backwards,distance);
        }
        stree->smallnotcompleted = 0;
        stree->chainstart = NULL;
    }
    stree->nextfreebranch += LARGEINTS;
    stree->nextfreebranchnum += LARGEINTS;
    stree->largenode++;
}

/*
   The function \emph{linkrootchildren} constructs the successor chain
   for the children of the root. This is done at the end of the algorithm
   in one sweep over table \emph{rootchildren}.
   */

static void linkrootchildren(Suffixtree *stree)
{
    Uint *rcptr, *prevnodeptr, prev = UNDEFREFERENCE;

    DEBUGDEFAULT;
    stree->alphasize = 0;
    for(rcptr = stree->rootchildren;
            rcptr <= stree->rootchildren + MAX_CHARS; rcptr++)
    {
        if(*rcptr != UNDEFREFERENCE)
        {
            stree->alphasize++;
            if(prev == UNDEFREFERENCE)
            {
                SETCHILD(stree->branchtab,MAKELARGE(*rcptr));
            } else
            {
                if(ISLEAF(prev))
                {
                    stree->leaftab[GETLEAFINDEX(prev)] = *rcptr;
                } else
                {
                    prevnodeptr = stree->branchtab + GETBRANCHINDEX(prev);
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
        prevnodeptr = stree->branchtab + GETBRANCHINDEX(prev);
        SETBROTHER(prevnodeptr,MAKELEAF(stree->textlen));
    }
    stree->leaftab[stree->textlen] = NILBIT;
}


static void initSuffixtree(Suffixtree *stree,SYMBOL *text,Uint textlen)
{
    Uint i;

    stree->currentbranchtabsize
        = (Uint) (STARTFACTOR * MULTBYSMALLINTS(textlen+1));
    if(stree->currentbranchtabsize < MINEXTRA) {
        stree->currentbranchtabsize = MULTBYSMALLINTS(MINEXTRA);
    }

    stree->leaftab = ALLOCSPACE(NULL, Uint, textlen + 2);

    stree->branchtab = ALLOCSPACE(NULL, Uint, stree->currentbranchtabsize);
    for(i=0; i<LARGEINTS; i++) {
        stree->branchtab[i] = 0;
    }

    stree->rootchildren = ALLOCSPACE(NULL, Uint, MAX_CHARS + 1);
    for(Uint *child= stree->rootchildren; child<=stree->rootchildren + MAX_CHARS; child++)
    {
        *child = UNDEFREFERENCE;
    }

    stree->text = text;
    stree->tailptr = text;
    stree->textlen = textlen;
    stree->sentinel = text + textlen;
    stree->firstnotallocated
        = stree->branchtab + stree->currentbranchtabsize - LARGEINTS;
    stree->headnode = stree->nextfreebranch = stree->branchtab;
    stree->headend = NULL;
    stree->headnodedepth = stree->maxbranchdepth = 0;

    stree->nextfreebranch = stree->branchtab;
    stree->nextfreebranchnum = 0;

    SETDEPTHHEADPOS(0, 0);
    SETNEWCHILDBROTHER(MAKELARGELEAF(0),0);
    SETBRANCHNODEOFFSET;
    printf("%c\n", *text);
    stree->rootchildren[(Uint) *text] = MAKELEAF(0); // Necessary?
    stree->leaftab[0]                 = VALIDINIT;

    stree->leafcounts                 = NULL;
    stree->nextfreeleafnum            = 1;
    stree->nextfreeleafptr            = stree->leaftab + 1;
    stree->nextfreebranch             = stree->branchtab + LARGEINTS;
    stree->nextfreebranchnum          = LARGEINTS;
    stree->insertnode                 = UNDEFREFERENCE;
    stree->insertprev                 = UNDEFREFERENCE;
    stree->smallnotcompleted          = 0;
    stree->chainstart                 = NULL;
    stree->largenode                  = stree->smallnode              = 0;


#ifdef DEBUG
    stree->showsymbolstree = NULL;
    stree->alphabet = NULL;
    stree->nodecount = 1;
    stree->splitleafedge =
        stree->splitinternaledge =
        stree->artificial =
        stree->multiplications = 0;
    stree->insertleafcalls = 1;
    stree->maxset = stree->branchtab + LARGEINTS - 1;
    stree->largelinks = stree->largelinkwork = stree->largelinklinkwork = 0;
#endif

    //}

}

void freestree(Suffixtree *stree)
{
    FREESPACE(stree->leaftab);
    FREESPACE(stree->rootchildren);
    FREESPACE(stree->branchtab);
    if(stree->nonmaximal != NULL)
    {
        FREESPACE(stree->nonmaximal);
    }
    if(stree->leafcounts != NULL)
    {
        FREESPACE(stree->leafcounts);
    }
}

// The following are some disjoint sections for creating a suffix tree. First,
// the alias for CONSTRUCT is defined, together with some additional options.
// Then, the 'suffix tree function' is included as gen.c. Then, the
// stuff gets undefined. In effect, A bunch of suffix tree methods are defined,
// reusing the code in 'gen.c'


///////////////////////////////////////////////////////////////////////////////
// Normal tree


#define CONSTRUCT Sint constructstree(Suffixtree *stree,SYMBOL *text,Uint textlen)
#define DECLAREEXTRA        stree->nonmaximal = NULL
#define COMPLETELARGEFIRST  completelarge(stree)
#define COMPLETELARGESECOND completelarge(stree)
#define PROCESSHEAD         /* Nothing */
#define CHECKSTEP           /* Nothing */
#define FINALPROGRESS       /* Nothing */

#include "gen.c"

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGEFIRST
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS


///////////////////////////////////////////////////////////////////////////////
// Markmaxstree


#define CONSTRUCT Sint constructmarkmaxstree(Suffixtree *stree,SYMBOL *text,Uint textlen)

#define DECLAREEXTRA  Uint distance, headposition = 0, *largeptr,\
                                                    tabsize = 1 + DIVWORDSIZE(textlen+1), *tabptr;\
                                                    stree->nonmaximal = ALLOCSPACE(NULL,Uint,tabsize);\
                                                    for(tabptr = stree->nonmaximal;\
                                                            tabptr < stree->nonmaximal + tabsize;\
                                                            tabptr++)\
{\
    *tabptr = 0;\
}

#define COMPLETELARGEFIRST\
    GETONLYHEADPOS(headposition,stree->headnode);\
    DEBUG2(3,"j=%lu: slink->%lu\n",(Ulong) stree->nextfreeleafnum,\
            (Ulong) headposition);\
            SETIBIT(stree->nonmaximal,headposition);\
            completelarge(stree);

#define COMPLETELARGESECOND\
    DEBUG2(3,"j=%lu: slink->%lu\n",(Ulong) stree->nextfreeleafnum,\
            (Ulong) stree->nextfreeleafnum);\
            SETIBIT(stree->nonmaximal,stree->nextfreeleafnum);\
            completelarge(stree)

#define PROCESSHEAD          /* Nothing */
#define CHECKSTEP            /* Nothing */
#define FINALPROGRESS        /* Nothing */

#include "gen.c"

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGEFIRST
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS


///////////////////////////////////////////////////////////////////////////////
// Headstree


#define CONSTRUCT Sint constructheadstree(Suffixtree *stree,SYMBOL *text,Uint textlen,void(*processhead)(Suffixtree *,Uint,void *),void *processheadinfo)

#define DECLAREEXTRA         stree->nonmaximal = NULL
#define COMPLETELARGEFIRST   completelarge(stree)
#define COMPLETELARGESECOND  completelarge(stree)
#define PROCESSHEAD          processhead(stree,stree->nextfreeleafnum,\
        processheadinfo)

#define CHECKSTEP            /* Nothing */
#define FINALPROGRESS        /* Nothing */

#include "gen.c"

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGEFIRST
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS

///////////////////////////////////////////////////////////////////////////////
// Progresstree

#define LEASTSHOWPROGRESS 100000
#define NUMOFCALLS 100

#define CONSTRUCT Sint constructprogressstree(Suffixtree *stree,SYMBOL *text,Uint textlen,void (*progress)(Uint,void *),void (*finalprogress)(void *),void *info)
#define DECLAREEXTRA\
    Uint j = 0, step, nextstep;\
    stree->nonmaximal = NULL;\
    step = textlen/NUMOFCALLS;\
    nextstep = (textlen >= LEASTSHOWPROGRESS) ? step : (textlen+1);\
    if(progress == NULL && textlen >= LEASTSHOWPROGRESS)\
{\
    fprintf(stderr,"# process %lu characters per dot\n",\
            (Ulong) textlen/NUMOFCALLS);\
}

#define COMPLETELARGEFIRST  completelarge(stree)
#define COMPLETELARGESECOND completelarge(stree)
#define PROCESSHEAD          /* Nothing */

#define CHECKSTEP            if(j == nextstep)\
{\
    if(progress == NULL)\
    {\
        if(nextstep == step)\
        {\
            fputc('#',stderr);\
        }\
        fputc('.',stderr);\
        fflush(stdout);\
    } else\
    {\
        progress(nextstep,info);\
    }\
    nextstep += step;\
}\
j++

#define FINALPROGRESS        if(textlen >= LEASTSHOWPROGRESS)\
{\
    if(finalprogress == NULL)\
    {\
        fputc('\n',stderr);\
    } else\
    {\
        finalprogress(info);\
    }\
}

#include "gen.c"

#undef CONSTRUCT
#undef DECLAREEXTRA
#undef COMPLETELARGEFIRST
#undef COMPLETELARGESECOND
#undef PROCESSHEAD
#undef CHECKSTEP
#undef FINALPROGRESS

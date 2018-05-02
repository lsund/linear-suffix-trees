/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include "types.h"
#include "basedef.h"
#include "spaceman.h"

/*EE
  This file contains functions to store pointers to dynamically allocated
  spaceblocks, and to maintain the number of cells and their size in
  each block. The arguments \texttt{file} and \texttt{line} (if they occur)
  are always the filename and the linenumber the function is called from.
  To supply these arguments, we recommend to call the corresponding
  functions via some useful macros, as defined in the file
  \texttt{spaceman.h}.
  \begin{enumerate}
  \item
  The function \texttt{allocspaceviaptr} should be called
  via the macro \texttt{ALLOC}.
  \item
  The function \texttt{freespaceviaptr} should be called
  via the macro \texttt{FREE}.
  \item
  The function \texttt{dynamicstrdup} should be called
  via the macro \texttt{DYNAMICSTRDUP}.
  \end{enumerate}
  */

static Blockdescription *blocks = NULL;

static Uint numberofblocks = 0, // numberofblocks
            nextfreeblock = 0,  // index of next free block
            currentspace = 0,   // currently allocated num of bytes
            spacepeak = 0;      // maximally allocated num of bytes

/*
   The following function sets the soft limit on the data size to the hard
   limit, if it is not already identical to this.
   This is necessary for the DEC alpha platform, where the soft limit
   is too small. \texttt{setmaxspace} is called for the first time,
   space is allocated.
   */

static void setmaxspace(void)
{
    int rc;
    struct rlimit rls;

    if((rc = getrlimit(RLIMIT_DATA,&rls)) != 0) {
        fprintf(stderr,"cannot find rlimit[RLIMIT_DATA]\n");
        exit(EXIT_FAILURE);
    }

    if(rls.rlim_cur < rls.rlim_max) {
        rls.rlim_cur = rls.rlim_max;
        if((rc = setrlimit(RLIMIT_DATA, &rls)) != 0) {
            fprintf(stderr,"cannot set rlimit[RLIMIT_DATA]\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void addspace(Uint space) {
    if(currentspace == 0) {
        setmaxspace();
        currentspace = space;
    } else {
        currentspace += space;
    }
    if(currentspace > spacepeak) {
        spacepeak = currentspace;
    }
}

static void subtractspace(Uint space) {
    currentspace -= space;
}

/*EE
  The following function allocates \texttt{number} cells of \texttt{size}
  for a given pointer \texttt{ptr}. If this is \texttt{NULL}, then the next
  free block is used. Otherwise, we look for the block number corresponding
  to \texttt{ptr}. If there is none, then the program exits with exit code 1.
  */

void *alloc_use(Uint line, void *ptr, Uint size,Uint number)
{
    Uint i, blocknum;

    if(nextfreeblock > 0) {
        if (!blocks) {
            fprintf(stderr, "Not supposed to be null");
        }
        for(blocknum=0; blocknum < nextfreeblock; blocknum++)
        {
            if(blocks[blocknum].spaceptr == ptr)
            {
                break;
            }
        }
    } else {
        blocknum = 0;
    } if(blocknum == nextfreeblock) {
        if(ptr == NULL) {
            nextfreeblock += 64;
            blocks = (Blockdescription *) realloc(blocks,
                    (size_t) (sizeof(Blockdescription)*
                        nextfreeblock));
            if(blocks == NULL) {
                fprintf(stderr, "not enough space for the block descriptions available");
                exit(EXIT_FAILURE);
            }
            for(i=blocknum; i < nextfreeblock; i++)
            {
                blocks[i].spaceptr = NULL;
                blocks[i].sizeofcells = 0;
                blocks[i].numberofcells = 0;
            }
        } else
        {
            fprintf(stderr, "cannot find space block");
            exit(EXIT_FAILURE);
        }
    }
    if (!blocks) {
        fprintf(stderr, "Not supposed to be null");
    }
    subtractspace(blocks[blocknum].numberofcells * blocks[blocknum].sizeofcells);
    addspace(size*number);
    blocks[blocknum].numberofcells = number;
    blocks[blocknum].sizeofcells = size;
    blocks[blocknum].lineallocated = line;
    if(blocks[blocknum].spaceptr == NULL) {
        numberofblocks++;
    }
    if((blocks[blocknum].spaceptr
                = realloc(blocks[blocknum].spaceptr,(size_t) (size*number))) == NULL)
    {
        fprintf(stderr, "not enough memory");
        exit(EXIT_FAILURE);
    }
    if (!blocks[blocknum].spaceptr) {
        fprintf(stderr, "Not supposed to be null");
    }
    return blocks[blocknum].spaceptr;
}

/*EE
  The following function frees the space for the given pointer
  \texttt{ptr}. This cannot be \texttt{NULL}.
  */

void freespaceviaptr(char *file,Uint line,void *ptr)
{
    Uint blocknum;

    if(ptr == NULL)
    {
        fprintf(stderr,"freespaceviaptr(file=%s,line=%lu): Cannot free NULL-ptr\n",
                file,(Ulong) line);
        exit(EXIT_SUCCESS);
    }
    if (!blocks) {
        fprintf(stderr, "Not supposed to be null");
    }
    for(blocknum=0; blocknum < nextfreeblock; blocknum++)
    {
        if(blocks[blocknum].spaceptr == ptr)
        {
            break;
        }
    }
    if(blocknum == nextfreeblock)
    {
        fprintf(stderr,"freespaceviaptr(file=%s,line=%lu): "
                " cannot find space block\n",
                file,(Ulong) line);
        exit(EXIT_FAILURE);
    }
    free(blocks[blocknum].spaceptr);
    subtractspace(blocks[blocknum].numberofcells * blocks[blocknum].sizeofcells);
    blocks[blocknum].numberofcells = 0;
    blocks[blocknum].sizeofcells = 0;
    blocks[blocknum].lineallocated = 0;
    blocks[blocknum].spaceptr = NULL;
    if(numberofblocks == 0)
    {
        fprintf(stderr, "Not supposed to happen");
    }
    numberofblocks--;
}

/*EE
  The following function frees the space for all main memory blocks
  which have not already been freed.
  */

void wrapspace(void)
{
    Uint blocknum;

    if (!blocks) {
        fprintf(stderr, "Not supposed to be null");
    }
    for(blocknum=0; blocknum < nextfreeblock; blocknum++)
    {
        if(blocks[blocknum].spaceptr != NULL)
        {
            free(blocks[blocknum].spaceptr);
            blocks[blocknum].spaceptr = NULL;
        }
        subtractspace(blocks[blocknum].sizeofcells *
                blocks[blocknum].numberofcells);
        blocks[blocknum].sizeofcells = 0;
        blocks[blocknum].numberofcells = 0;
        blocks[blocknum].lineallocated = 0;
    }
}

//}

/*EE
  The following function prints a list of block numbers
  which have not been freed. For each block number the filename
  and line number in which the call appears allocating which
  allocated this block.
  */

void activeblocks(void)
{
    Uint blocknum;

    if (!blocks) {
        fprintf(stderr, "Not supposed to be null");
    }
    for(blocknum=0; blocknum < nextfreeblock; blocknum++)
    {
        if(blocks[blocknum].spaceptr != NULL)
        {
            fprintf(stderr,"# active block %lu: ",(Ulong) blocknum);
            fprintf(stderr,"line %lu\n",
                    (Ulong) blocks[blocknum].lineallocated);
        }
    }
}

/*EE
  The following function checks if all blocks previously allocated, have
  explicitely been freed. If there is a block that was not freed, then
  an error is reported accordingly. We recommend to call this function
  before the program terminates. This easily allows to discover
  space leaks.
  */

void checkspaceleak(void)
{
    Uint blocknum;

    if (!blocks) {
        fprintf(stderr, "Not supposed to be null");
    }

    for(blocknum=0; blocknum < nextfreeblock; blocknum++)
    {
        if(blocks[blocknum].spaceptr != NULL)
        {
            fprintf(stderr,"space leak: main memory for block %lu not freed\n",
                    (Ulong) blocknum);
            fprintf(stderr,"%lu cells of size %lu\n",
                    (Ulong) blocks[blocknum].numberofcells,
                    (Ulong) blocks[blocknum].sizeofcells);
            fprintf(stderr,"allocated: ");
            exit(EXIT_FAILURE);
        }
    }
    if(numberofblocks > 0)
    {
        fprintf(stderr,"space leak: number of blocks = %lu\n", numberofblocks);
        exit(EXIT_FAILURE);
    }
    free(blocks);
    blocks = NULL;
    numberofblocks = 0;
    nextfreeblock = 0;
    currentspace = 0;
    spacepeak = 0;
}

/*EE
  The following function shows the space peak in megabytes on \texttt{stderr}.
  */

void showspace(void)
{
    fprintf(stderr,"# space peak in megabytes: %.2f\n",MEGABYTES(spacepeak));
}

/*EE
  The following function returns the space peak in bytes.
  */

Uint getspacepeak(void)
{
    return spacepeak;
}

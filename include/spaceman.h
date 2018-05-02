/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef SPACEMAN_H
#define SPACEMAN_H
#include "types.h"
#include "util.h"

typedef struct {
    void *spaceptr;      // ptr to the spaceblock
    Uint sizeofcells,    // size of cells of the block
         numberofcells;  // number of cells in the block
    Uint lineallocated;  // the linenumber where the
} Blockdescription;

void *alloc_use(Uint line, void *ptr, Uint size, Uint number);

void freespaceviaptr(char *file,Uint line,void *ptr);

Uint getspacepeak(void);

char *messagespace(void);

#define ALLOC(S,T,N)\
        (T *) alloc_use((Uint) __LINE__, S,(Uint) sizeof(T),N)

#define FREE(P)\
        if((P) != NULL)\
        {\
          freespaceviaptr(__FILE__,(Uint) __LINE__,P);\
          P = NULL;\
        }

#endif

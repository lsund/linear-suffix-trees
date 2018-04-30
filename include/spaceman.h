/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef SPACEDEF_H
#define SPACEDEF_H
#include "types.h"

void *allocandusespaceviaptr(
        char *file,Uint line,
        void *ptr,
        Uint size,
        Uint number
    );

void freespaceviaptr(char *file,Uint line,void *ptr);

Uint getspacepeak(void);

char *messagespace(void);

#define ALLOC(S,T,N)\
        (T *) allocandusespaceviaptr(__FILE__,(Uint) __LINE__,\
                                     S,(Uint) sizeof(T),N)

#define FREE(P)\
        if((P) != NULL)\
        {\
          freespaceviaptr(__FILE__,(Uint) __LINE__,P);\
          P = NULL;\
        }

#endif

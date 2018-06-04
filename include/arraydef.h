/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef ARRAYDEF_H
#define ARRAYDEF_H


#include "types.h"
#include "spaceman.h"

#define DECLAREARRAYSTRUCT(TYPE)\
        typedef struct\
        {\
          TYPE *space##TYPE;\
          Uint allocated##TYPE, nextfree##TYPE;\
        } Array##TYPE


#define INITARRAY(A,TYPE)\
        (A)->space##TYPE = NULL;\
        (A)->allocated##TYPE = (A)->nextfree##TYPE = 0


#define CHECKARRAYSPACE(A,TYPE,L)\
        if((A)->nextfree##TYPE >= (A)->allocated##TYPE)\
        {\
          (A)->allocated##TYPE += L;\
          (A)->space##TYPE\
             = (TYPE *) alloc_use((Uint) __LINE__,\
                                               (A)->space##TYPE,\
                                               (Uint) sizeof(TYPE),\
                                               (A)->allocated##TYPE);\
        }\
        if (!(A)->space##TYPE) {\
            fprintf(stderr, "Not supposed to be null");\
        }\


#define STOREINARRAY(A,TYPE,L,VAL)\
        CHECKARRAYSPACE(A,TYPE,L);\
        (A)->space##TYPE[(A)->nextfree##TYPE++] = VAL


#define FREEARRAY(A,TYPE)\
        if((A)->space##TYPE != NULL)\
        {\
          FREE((A)->space##TYPE);\
        }


DECLAREARRAYSTRUCT(Wchar);
DECLAREARRAYSTRUCT(Uint);

typedef ArrayWchar ArrayCharacters;

#endif

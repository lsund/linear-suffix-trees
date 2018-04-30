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
#include "errordef.h"
#include "spaceman.h"

#define DECLAREARRAYSTRUCT(TYPE)\
        typedef struct\
        {\
          TYPE *space##TYPE;\
          Uint allocated##TYPE, nextfree##TYPE;\
        } Array##TYPE

/*
  \texttt{INITARRAY} initializes an empty array.
*/

#define INITARRAY(A,TYPE)\
        (A)->space##TYPE = NULL;\
        (A)->allocated##TYPE = (A)->nextfree##TYPE = 0

/*
  \texttt{CHECKARRAYSPACE} checks if the integer \texttt{nextfree\#\#T}
  points to an index for which the space is not allocated yet. If this is
  the case, the number of cells allocated is incremented by \texttt{L}. The
  contents of the previously filled array elements is of course maintained.
*/

#define CHECKARRAYSPACE(A,TYPE,L)\
        if((A)->nextfree##TYPE >= (A)->allocated##TYPE)\
        {\
          (A)->allocated##TYPE += L;\
          (A)->space##TYPE\
             = (TYPE *) allocandusespaceviaptr(__FILE__,(Uint) __LINE__,\
                                               (A)->space##TYPE,\
                                               (Uint) sizeof(TYPE),\
                                               (A)->allocated##TYPE);\
        }\
        NOTSUPPOSEDTOBENULL((A)->space##TYPE)

/*
  The next macro is a variation of \texttt{CHECKARRAYSPACE},
  which checks if the next
  \texttt{L} cells have been allocated. If not, then this is done.
*/

#define CHECKARRAYSPACEMULTI(A,TYPE,L)\
        if((A)->nextfree##TYPE + (L) >= (A)->allocated##TYPE)\
        {\
          (A)->allocated##TYPE += L;\
          (A)->space##TYPE\
             = (TYPE *) allocandusespaceviaptr(__FILE__,(Uint) __LINE__,\
                                               (A)->space##TYPE,\
                                               (Uint) sizeof(TYPE),\
                                               (A)->allocated##TYPE);\
        }\
        NOTSUPPOSEDTOBENULL((A)->space##TYPE)

/*
  This macro checks the space and delivers a pointer \texttt{P}
  to the next free element in the array.
*/

#define GETNEXTFREEINARRAY(P,A,TYPE,L)\
        CHECKARRAYSPACE(A,TYPE,L);\
        P = (A)->space##TYPE + (A)->nextfree##TYPE++;

/*
  This macro checks the space and stores \texttt{V} in the
  \texttt{nextfree}-component of the array. \texttt{nextfree}
  is incremented.
*/

#define STOREINARRAY(A,TYPE,L,VAL)\
        CHECKARRAYSPACE(A,TYPE,L);\
        (A)->space##TYPE[(A)->nextfree##TYPE++] = VAL

/*
  This macro frees the space for an array if it is not \texttt{NULL}.
*/

#define FREEARRAY(A,TYPE)\
        if((A)->space##TYPE != NULL)\
        {\
          FREE((A)->space##TYPE);\
        }

/*
  Some declarations for the most common array types.
*/

DECLAREARRAYSTRUCT(wchar_t);
DECLAREARRAYSTRUCT(Ushort);
DECLAREARRAYSTRUCT(char);
DECLAREARRAYSTRUCT(Uint);
DECLAREARRAYSTRUCT(Sint);
DECLAREARRAYSTRUCT(PairUint);
DECLAREARRAYSTRUCT(ThreeUint);

/*
  And some type synonyms.
*/

typedef ArrayUint  ArrayPosition;       // \Typedef{ArrayPosition}
typedef Arraywchar_t ArrayCharacters;     // \Typedef{ArrayCharacters}

/*
  The following array type has some extra components. However, it can be
  manipulated by the macros above since the record-components
  \texttt{spaceStrings}, \texttt{nextfreeStrings}, and
  \texttt{allocatedStrings} is declared appropriately.
*/

typedef struct
{
  String *spaceStrings;
  wchar_t *stringbuffer;
  Uint stringbufferlength, nextfreeStrings, allocatedStrings;
} ArrayStrings;   // \Typedef{ArrayStrings}

//\Ignore{

#endif

//}

/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

// This is the assumptions about the types
// size(Uint) >= 4
// size(Sint) >= 4
// size(Ushort) = 2
// size(Sshort) = 2


// Primitives
typedef unsigned char  Uchar;
typedef unsigned short Ushort;

typedef unsigned long  Uint;
typedef signed   long  Sint;

typedef unsigned long  Ulong;
typedef signed   long  Slong;

#define LOGWORDSIZE    6
#define UintConst(N)   (N##UL)
#define SintConst(N)   (N##L)

#define Bool unsigned char

#define False ((Bool) 0)

#define True ((Bool) 1)

#ifdef WITHSYSCONF
typedef int Sysconfargtype;         // \Typedef{Sysconfargtype}
#endif

// Fast division
#define DIV2(N)      ((N) >> 1)
#define DIV4(N)      ((N) >> 2)
#define DIV8(N)      ((N) >> 3)

// Fast multiplication
#define MULT2(N)     ((N) << 1)
#define MULT4(N)     ((N) << 2)
#define MULT8(N)     ((N) << 3)

// Fast modulu
#define MOD2(N)      ((N) & 1)
#define MOD4(N)      ((N) & 3)
#define MOD8(N)      ((N) & 7)

//\IgnoreLatex{

#define CHECKTYPESIZE(T,OP,S)\
        if(sizeof(T) OP (S))\
        {\
          DEBUG4(1,"# sizeof(%s) %s (%ld bytes,%ld bits) as epected\n",\
                  #T,#OP,(Slong) sizeof(T),\
                         (Slong) (CHAR_BIT * sizeof(T)));\
        } else\
        {\
          fprintf(stderr,"typesize constraint\n");\
          fprintf(stderr,"  sizeof(%s) = (%ld bytes,%ld bits) %s %lu bytes\n",\
                  #T,\
                  (Slong) sizeof(T),\
                  (Slong) (CHAR_BIT * sizeof(T)),\
                  #OP,\
                  (Ulong) (S));\
          fprintf(stderr,"does not hold\n");\
          exit(EXIT_FAILURE);\
        }

/*
  The following function checks some type constraints
*/

#define CHECKALLTYPESIZES\
        CHECKTYPESIZE(char,==,(size_t) 1)\
        CHECKTYPESIZE(short,==,(size_t) 2)\
        CHECKTYPESIZE(int,==,(size_t) 4)\
        CHECKTYPESIZE(long,>=,(size_t) 4)\
        CHECKTYPESIZE(void *,>=,(size_t) 4)

//}

/*
  A type for boolean values defined as a constant to allow
  checking if it has been defined previously.
*/

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef False
#define False ((BOOL) 0)
#endif

#ifndef True
#define True ((BOOL) 1)
#endif

/*
  Show a boolean value as a string or as a character 0 or 1.
*/

#define SHOWBOOL(B) ((B) ? "True" : "False")
#define SHOWBIT(B)  ((B) ? '1' : '0')

/*
  Pairs, triples, and quadruples of unsigned integers.
*/

typedef struct
{
  Uint uint0, uint1;
} PairUint;                // \Typedef{PairUint}

typedef struct
{
  Uint uint0, uint1, uint2;
} ThreeUint;               // \Typedef{ThreeUint}

typedef struct
{
  Uint uint0, uint1, uint2, uint3;
} FourUint;                // \Typedef{FourUint}

//\IgnoreLatex{

/*
  A list is stored with its start position in some space block
  and its length.
*/

typedef struct
{
  Uint start, length;
} Listtype;                // \Typedef{Listtype}

/*
  A string is just a list.
*/

typedef Listtype Stringtype;    // \Typedef{Stringtype}

/*
  The default type for length-values is unsigned int.
*/

#ifndef LENGTHTYPE
#define LENGTHTYPE Uint
#endif

/*
  The default number of bytes in a bitvector used for dynamic programming
  is 4.
*/

#ifndef DPBYTESINWORD
#define DPBYTESINWORD 4
#endif

/*
  The number of bytes in a dynamic programming bitvector determines the type
  of integers, the dp-bits are stored in.
*/

#if DPBYTESINWORD == 1
typedef unsigned char DPbitvector;          // \Typedef{DPbitvector}
#else
#if DPBYTESINWORD == 2
typedef unsigned short DPbitvector;
#else
#if DPBYTESINWORD == 4
typedef unsigned int DPbitvector;
#else
#if DPBYTESINWORD == 8
typedef unsigned long long DPbitvector;
#endif
#endif
#endif
#endif

typedef unsigned int DPbitvector4;          // \Typedef{DPbitvector4}

#if (LOGWORDSIZE==6)
typedef unsigned long DPbitvector8;         // \Typedef{DPbitvector8}
#endif

//}

/*
  The following type stores filenames and the length of the corresponding
  files.
*/

typedef struct
{
  char *filenamebuf;    // pointer to a copy of a filename
  Uint filelength;      // the length of the corresponding file
} Fileinfo;             // \Typedef{Fileinfo}

/*
  The following is the type of the comparison function
  to be provided to the function \texttt{qsort}.
*/

typedef int (*Qsortcomparefunction)(const void *,const void *);

//\IgnoreLatex{

#endif

//}

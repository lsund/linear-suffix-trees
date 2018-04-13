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
// size(char) = 1
// size (Ulong) >= 4
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

//  Pair of Uint
typedef struct {
  Uint uint0, uint1;
} PairUint;

// Triple of Uint
typedef struct {
  Uint uint0, uint1, uint2;
} ThreeUint;

// A struct representing a string, with its start and length
typedef struct {
  Uint start, length;
} String;

//}

// Information about a file
typedef struct {
  char *filenamebuf;    // pointer to a copy of a filename
  Uint filelength;      // the length of the corresponding file
} Fileinfo;             // \Typedef{Fileinfo}

/*
  The following is the type of the comparison function
  to be provided to the function \texttt{qsort}.
*/

#endif

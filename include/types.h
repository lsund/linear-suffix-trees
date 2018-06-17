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
typedef wchar_t Wchar;
typedef unsigned char  Uchar;
typedef unsigned short Ushort;

typedef unsigned long  Uint;
typedef signed   long  Sint;

typedef unsigned long  Ulong;
typedef signed   long  Slong;

#define UintConst(N)   (N##UL)

#endif

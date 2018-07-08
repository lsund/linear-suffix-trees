#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

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

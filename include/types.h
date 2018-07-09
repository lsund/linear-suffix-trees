#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

// Primitive type definitions
typedef unsigned char  Uchar;
typedef unsigned short Ushort;
typedef unsigned long  Uint;
typedef signed   long  Sint;
typedef unsigned long  Ulong;
typedef signed   long  Slong;
typedef wchar_t        Wchar;
typedef Uint Vertex;

// Reference to a vertex
typedef Uint *VertexP;

// Unsigned integer constructor
#define UINT(N)   (N##UL)

#endif

#ifndef BASEDEF_H
#define BASEDEF_H

#include "bitvector.h"

#define MAX_ALLOC           5000000 // 5 MB
/* #define MAX_ALLOC           100000000 // 100 MB */
/* #define MAX_ALLOC           150000000 // 150 MB */
/* #define MAX_ALLOC           450000000 // 450 MB */

///////////////////////////////////////////////////////////////////////////////
// Fields

#define ITER                    1

#define BRANCHWIDTH             UintConst(2)

/* #define MAXTEXTLEN              UintConst(715827882) */

#define MAXPATTERNLEN           1024

// undefined reference
/* #define UNDEFREFERENCE          UINT_MAX */
#define UNDEFREFERENCE      (~((Uint) 0))

// undefined successor
#define UNDEFINEDSUCC           UINT_MAX

// Transform bytes into megabytes
#define MEGABYTES(V)            ((double) (V)/((UintConst(1) << 20) - 1))

#define MAX_CHARS               100000
#define MAX_PATTERNS 20000


// Fast division
#define DIV2(N)      ((N) >> 1)
#define DIV4(N)      ((N) >> 2)
#define DIV8(N)      ((N) >> 3)

// Fast multiplication
#define MULT2(N)     ((N) << 1)
#define MULT4(N)     ((N) << 2)
#define MULT8(N)     ((N) << 3)

// Fast modulo
#define MOD2(N)      ((N) & 1)
#define MOD4(N)      ((N) & 3)
#define MOD8(N)      ((N) & 7)

#if WcharBYTES == 1
#define LARGESTCHARINDEX          MAX_CHARS
#else
#define LARGESTCHARINDEX          stree->lastcharindex
#endif

#define MEGABYTES(V)  ((double) (V)/((UintConst(1) << 20) - 1))

#endif

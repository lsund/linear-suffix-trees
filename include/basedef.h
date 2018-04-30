#ifndef PREPROCDEF_H
#define PREPROCDEF_H


///////////////////////////////////////////////////////////////////////////////
// Fields

#define ITER                    1

#define BRANCHWIDTH             UintConst(2)

#define MAXTEXTLEN              UintConst(715827882)

#define MAXPATTERNLEN           1024

// undefined reference
/* #define UNDEFREFERENCE          UINT_MAX */
#define UNDEFREFERENCE      (~((Uint) 0))

// undefined successor
#define UNDEFINEDSUCC           UINT_MAX

// Transform bytes into megabytes
#define MEGABYTES(V)            ((double) (V)/((UintConst(1) << 20) - 1))

#define MAX_CHARS               UCHAR_MAX

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

#if SYMBOLBYTES == 1
#define LARGESTCHARINDEX          UCHAR_MAX
#else
#define LARGESTCHARINDEX          stree->lastcharindex
#endif


#endif

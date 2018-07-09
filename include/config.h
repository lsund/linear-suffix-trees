#ifndef BASEDEF_H
#define BASEDEF_H

#define MAX_ALLOC           450000000 // 450 MB

#define BRANCHWIDTH             UINT(2)

#define MAX_PATTERNLEN           100000

#define MAX_CHARS               100000

#define MAX_PATTERNS            100000

#define LOGWORDSIZE    6

#define INTWORDSIZE\
        (UINT(1) << LOGWORDSIZE)     // # of bits in Uint = w

#define LOCALE                  "en_US.utf8"

#endif

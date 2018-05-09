/*
  Copyright by Stefan Kurtz (C) 1997-2003
  =====================================
  You may use, copy and distribute this file freely as long as you
   - do not change the file,
   - leave this copyright notice in the file,
   - do not make any profit with the distribution of this file
   - give credit where credit is due
  You are not allowed to copy or distribute this file otherwise
  The commercial usage and distribution of this file is prohibited
  Please report bugs and suggestions to <kurtz@zbh.uni-hamburg.de>
*/

//\IgnoreLatex{

#ifndef BITVECTOR_H
#define BITVECTOR_H
#include <limits.h>
#include "types.h"
#include "spaceman.h"


#define INTWORDSIZE\
        (UintConst(1) << LOGWORDSIZE)     // # of bits in Uint = w
#define MSB\
        (UintConst(1) << (INTWORDSIZE - 1)) // \(10^{w-1}\)
#define ISBITSET(S,I)\
        (((S) << (I)) & MSB)         // is \(i\)th bit set?
#define ITHBIT(I)\
        (MSB >> (I))                 // \(0^{i}10^{w-i-1}\)
#define SECOND_MSB\
        (MSB >> 1)                   // \(010^{w-2}\)
#define THIRDBIT\
        (MSB >> 2)                   // \(0010^{w-3}\)
#define FOURTHBIT\
        (MSB >> 3)                   // \(00010^{w-4}\)
#define FIFTHBIT\
        (MSB >> 4)                   // \(000010^{w-3}\)
#define FIRSTTWOBITS\
        (UintConst(3) << (INTWORDSIZE-2)) // \(11^{w-2}\)
#define EXCEPTFIRSTBIT\
        (~MSB)                       // \(01^{w-1}\)
#define EXCEPTFIRSTTWOBITS\
        (EXCEPTFIRSTBIT >> 1)             // \(001^{w-2}\)
#define EXCEPTFIRSTTHREEBITS\
        (EXCEPTFIRSTBIT >> 2)             // \(0001^{w-3}\)
#define EXCEPTFIRSTFOURBITS\
        (EXCEPTFIRSTBIT >> 3)             // \(00001^{w-4}\)
#define DIVWORDSIZE(I)\
        ((I) >> LOGWORDSIZE)              // \((I) div w\)
#define MODWORDSIZE(I)\
        ((I) & (INTWORDSIZE-1))           // \((I) mod w\)
#define MULWORDSIZE(I)\
        ((I) << LOGWORDSIZE)              // \((I) * w\)

/*
  The following defines the maximal value which can be represented by
  a given number of bits.
*/

#define MAXVALUEWITHBITS(BITNUM)       ((UintConst(1) << (BITNUM)) - 1)

/*
  The following defines the number of integers for a bitvector with N bits.
*/

#define NUMOFINTSFORBITS(N)\
        ((DIVWORDSIZE(N) == 0)\
           ? UintConst(1) \
           : (UintConst(1) + DIVWORDSIZE((N) - UintConst(1))))

#define INITBITTABGENERIC(TAB,OLDTAB,NUMOFBITS)\
        {\
          Uint *tabptr, tabsize = NUMOFINTSFORBITS(NUMOFBITS);\
          ALLOC(TAB,OLDTAB,Uint,tabsize);\
          for(tabptr = TAB; tabptr < (TAB) + tabsize; tabptr++)\
          {\
            *tabptr = 0;\
          }\
        }

#define INITBITTAB(TAB,N) INITBITTABGENERIC(TAB,NULL,N)

/*
  The following macro inititalizes a bitarray such that all bits
  are off.
*/

#define CLEARBITTAB(TAB,N)\
        {\
          Uint *tabptr, tabsize = NUMOFINTSFORBITS(N);\
          for(tabptr = TAB; tabptr < TAB + tabsize; tabptr++)\
          {\
            *tabptr = 0;\
          }\
        }


#define SETIBIT(TAB,I)    (TAB)[DIVWORDSIZE(I)] |= ITHBIT(MODWORDSIZE(I))

#define UNSETIBIT(TAB,I)  (TAB)[DIVWORDSIZE(I)] &= ~(ITHBIT(MODWORDSIZE(I)))

#define ISIBITSET(TAB,I)  ((TAB)[DIVWORDSIZE(I)] & ITHBIT(MODWORDSIZE(I)))


char *intbits2string(Uint bs);

#endif

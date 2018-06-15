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

typedef Uint *Bref;

typedef struct {
    Bref *spaceBref;
    Uint allocatedBref, nextfreeBref;
} ArrayBref;

typedef struct {
    Uint *spaceUint;
    Uint allocatedUint, nextfreeUint;
} ArrayUint;

#endif

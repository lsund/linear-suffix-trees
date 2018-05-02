/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include <stdio.h>
#include <time.h>
#include "types.h"


// Start and stoptime
static clock_t startclock, stopclock;


// Initialize the clock
void initclock(void)
{
  startclock = clock();
}


// The time since th clock was initialized
double getruntime(void)
{
   stopclock = clock();
   return (double) (stopclock - startclock) / (double) CLOCKS_PER_SEC;
}


// The clockticks since the clock was initialized
Uint getclockticks(void)
{
   stopclock = clock();
   return (Uint) (stopclock - startclock);
}

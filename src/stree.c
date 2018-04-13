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
#include <stdlib.h>
#include <sys/types.h>
#include "debugdef.h"
#include "args.h"
#include "protodef.h"
#include "errordef.h"
#include "streedef.h"
#include "spaceman.h"
#include "megabytes.h"
#include "clock.h"
#include "mapfile.h"

// This constructs a suffix tree
int main(int argc, char *argv[])
{
    Uchar *text;
    Uint textlen;
    Suffixtree stree;
    char *filename;

    DEBUGLEVELSET;

    CHECKARGNUM(2,"filename");
    initclock();
    filename = argv[1];
    text = (Uchar *) CREATEMEMORYMAP(filename,False,&textlen);
    if(text == NULL)
    {
        fprintf(stderr,"%s: cannot open file \"%s\" ",argv[0],filename);
        fprintf(stderr,"or file \"%s\" is empty\n",filename);
        return EXIT_FAILURE;
    }
    if(textlen == 0)
    {
        fprintf(stderr,"%s: file \"%s\" is empty\n",argv[0],filename);
        return EXIT_FAILURE;
    }
    fprintf(stderr,"# construct suffix tree for sequence of length %lu\n",
            (Ulong) textlen);
    fprintf(stderr,"# (maximal input length is %lu)\n",
            (Ulong) getmaxtextlenstree());
    if(constructprogressstree(&stree,text,textlen,NULL,NULL,NULL) != 0) {
        fprintf(stderr,"%s %s: %s\n",argv[0],filename,messagespace());
        return EXIT_FAILURE;
    }
    /*
       addleafcountsstree(&stree);
       */
    if(DELETEMEMORYMAP(text) != 0)
    {
        STANDARDMESSAGE;
    }
    freestree(&stree);
    fprintf(stderr,"# TIME %s %s %.2f\n",argv[0],filename,getruntime());
    fprintf(stderr,"# SPACE %s %s %.1f\n",argv[0],filename,
            (double) MEGABYTES(getspacepeak()));
    fprintf(stderr,"# MMSPACE %s %s %.1f\n",argv[0],filename,
            (double) MEGABYTES(mmgetspacepeak()));
    return EXIT_SUCCESS;
}

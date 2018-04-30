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
#include <wchar.h>
#include "debug.h"
#include "protodef.h"
#include "streedef.h"
#include "spaceman.h"
#include "megabytes.h"
#include "clock.h"
#include "mapfile.h"
#include "streelarge.h"
#include "externs.h"

wchar_t *text;

// This constructs a suffix tree
int main(int argc, char *argv[])
{
    Uint textlen = 0;
    Suffixtree stree;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Need exactly one argument");
        return EXIT_FAILURE;
    }

    DEBUGLEVELSET;

    initclock();
    filename = argv[1];

    FILE *in = fopen(filename, "r");
    text = malloc(sizeof(wchar_t) * MAXTEXTLEN);

    if(text == NULL) {
        fprintf(stderr,"%s: cannot open file \"%s\" ",argv[0],filename);
        fprintf(stderr,"or file \"%s\" is empty\n",filename);
        return EXIT_FAILURE;
    }

    Uint c;
    textlen = 0;
    while ((c = fgetwc(in)) != WEOF) {
        text[textlen] = c;
        textlen++;
    }
    text[textlen + 1] = '\0';

    if(textlen == 0) {
        fprintf(stderr,"%s: file \"%s\" is empty\n",argv[0],filename);
        return EXIT_FAILURE;
    }
    fprintf(stderr,"Creating a suffix tree for text of length %lu\n", textlen);
    fprintf(
        stderr,
        "%f %% of maximal input length \n",
        (float) 100 * textlen / (float) getmaxtextlenstree()
    );

    if(constructprogressstree(&stree,text,textlen,NULL,NULL,NULL) != 0) {
        fprintf(stderr,"%s %s: %s\n",argv[0],filename, messagespace());
        return EXIT_FAILURE;
    }
    /*
       addleafcountsstree(&stree);
       */
    freestree(&stree);

    fprintf(stderr,"# TIME %s %s %.2f\n",argv[0],filename,getruntime());
    fprintf(stderr,"# SPACE %s %s %.1f\n",argv[0],filename,
            (double) MEGABYTES(getspacepeak()));
    fprintf(stderr,"# MMSPACE %s %s %.1f\n",argv[0],filename,
            (double) MEGABYTES(mmgetspacepeak()));
    return EXIT_SUCCESS;
}

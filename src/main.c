/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */


#include "stree.h"

wchar_t *text;

// This constructs a suffix tree
int main(int argc, char *argv[])
{
    Uint textlen = 0;
    Suffixtree stree;
    char *filename;

    if (argc != 3) {
        fprintf(stderr, "Need exactly two arguments");
        return EXIT_FAILURE;
    }

    initclock();
    filename = argv[1];

    FILE *in = fopen(filename, "r");
    text = malloc(sizeof(wchar_t) * MAX_ALLOC);

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

    if (strcmp(argv[2], "stats") == 0) {
        if(constructprogressstree(&stree,text,textlen,NULL,NULL,NULL) != 0) {
            fprintf(stderr,"%s %s: %s\n",argv[0],filename, messagespace());
            return EXIT_FAILURE;
        }
    } else {
        if(constructstree(&stree,text,textlen) != 0) {
            fprintf(stderr,"%s", messagespace());
            return EXIT_FAILURE;
        }
    }
    freestree(&stree);

    fprintf(stderr,"# TIME %s %s %.2f\n",argv[0],filename,getruntime());
    fprintf(stderr,"# SPACE %s %s %.1f\n",argv[0],filename,
            (double) MEGABYTES(getspacepeak()));
    return EXIT_SUCCESS;
}



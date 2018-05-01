/*
 * Copyright (c) 2003 by Stefan Kurtz and The Institute for
 * Genomic Research.  This is OSI Certified Open Source Software.
 * Please see the file LICENSE for licensing information and
 * the file ACKNOWLEDGEMENTS for names of contributors to the
 * code base.
 *
 * Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.
 */

#include "construct.h"

wchar_t *text;

// This constructs a suffix tree
int main(int argc, char *argv[])
{
    Uint textlen = 0;
    Suffixtree stree;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Need exactly one arguments");
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

    if(constructstree(&stree,text,textlen) != 0) {
        fprintf(stderr,"Could not create suffix tree");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

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
#include "io.h"


wchar_t *text;
Uint textlen;


int main(int argc, char *argv[])
{
    STree stree;
    char *filename = argv[1];

    if (argc != 2) {
        fprintf(stderr, "Need exactly one argument");
        return EXIT_FAILURE;
    }

    file_to_string(filename);

    initclock();

    fprintf(stdout, "Creating a suffix tree for text of length %lu\n", textlen);

    if(construct(&stree, text, textlen) != 0) {
        fprintf(stderr,"Could not create suffix tree");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

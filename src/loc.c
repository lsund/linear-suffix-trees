/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wchar.h>
#include "debug.h"
#include "types.h"
#include "streedef.h"
#include "protodef.h"
#include "streeacc.h"
#include "spaceman.h"
#include "externs.h"
#include "streelarge.h"

wchar_t *text;

int main(int argc, char *argv[])
{
    Uint textlen;
    Suffixtree stree;

    DEBUGLEVELSET;

    if (argc != 2) {
        fprintf(stderr, "Need exactly one argument");
        return EXIT_FAILURE;
    }
    char *filename = argv[1];
    FILE *in = fopen(filename, "r");
    text = malloc(sizeof(wchar_t) * MAXTEXTLEN);
    Uint c;
    textlen = 0;
    while ((c = fgetwc(in)) != WEOF) {
        text[textlen] = c;
        textlen++;
    }
    text[textlen + 1] = '\0';
    if(text == NULL)
    {
        STANDARDMESSAGE;
    }
    CONSTRUCTSTREE(&stree,text,textlen,return EXIT_FAILURE);
#ifdef DEBUG
    enumlocations(&stree, checklocation);
#endif
    freestree(&stree);
    return EXIT_SUCCESS;
}

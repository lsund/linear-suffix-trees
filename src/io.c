/*
 * Copyright by Stefan Kurtz (C) 1999-2003
 * =====================================
 * You may use, copy and distribute this file freely as long as you
 * - do not change the file,
 * - leave this copyright notice in the file,
 * - do not make any profit with the distribution of this file
 * - give credit where credit is due
 * You are not allowed to copy or distribute this file otherwise
 * The commercial usage and distribution of this file is prohibited
 * Please report bugs and suggestions to <kurtz@zbh.uni-hamburg.de>
 *
 * ======================================
 *
 * Modified by Ludvig Sundström 2018 with permission from Stefan Kurtz
 * For full source control tree, see https://github.com/lsund/wotd
 *
 */

#include "io.h"

Wchar *text;
Uint textlen;


void file_to_string(const char *filename)
{
    FILE *in = fopen(filename, "r");
    text = malloc(sizeof(Wchar) * MAX_ALLOC);

    if(text == NULL) {
        fprintf(stderr,"Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    Uint c;
    textlen = 0;
    while ((c = fgetwc(in)) != WEOF) {
        text[textlen] = c;
        textlen++;
    }
    text[textlen + 1] = '\0';

    if(textlen == 0) {
        fprintf(stderr,"file \"%s\" is empty\n", filename);
        exit(EXIT_FAILURE);
    }
}


static int open_file(char *name, Uint *textlen, bool writefile)
{
    struct stat buf;
    int fd = open(name,(writefile) ? O_RDWR : O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "fileOpen: Cannot open \"%s\"", name);
        return EXIT_FAILURE;
    }

    int statres = fstat(fd,&buf);
    if (statres == -1) {
        fprintf(stderr, "fstat(fd = %d) failed", fd);
        return EXIT_FAILURE;
    }

    *textlen = (Uint) buf.st_size;
    return fd;
}

Uint file_to_strings(char *name, Uint *textlen, Uint nlines, Wchar ***wordsp)
{
    Wchar **words = *wordsp;
    int fd = open_file(name, textlen, false);

    if (fd < 0) {
        return -1;
    }
    int max_line_len = 1001;

    FILE *fp = fopen(name, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    Uint i;
    for (i = 0; i < nlines; i++)
    {
        Uint j;

        /* Allocate space for the next line */
        words[i] = (Wchar *) malloc(max_line_len * sizeof(Wchar));

        if (words[i] == NULL) {
            fprintf(stderr,"Out of memory (3).\n");
            exit(4);
        }

        wint_t c;
        j = 0;
        do  {
            c = fgetwc(fp);
            if (c == WEOF) {

                *wordsp = words;
                fclose(fp);
                return i;
            }
            words[i][j] = c;
            j++;
        } while (c != 10);

        words[i][j - 1] = 0;
    }
    fprintf(stderr, "Warning, not all patterns were read\n");
    *wordsp = words;
    fclose(fp);
    return i;
}


// Opens the path for appending, erasing any prior content of the same file
FILE *open_append(const char *path)
{
    fclose(fopen(path, WRITEMODE));
    return fopen(path, APPENDMODE);
}


// Frees the text specified
void freetextspace()
{
  (void) munmap((caddr_t) text, (size_t) textlen);
}



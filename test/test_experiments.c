#include "test.h"

static void print_statistics(FILE *out, STree st, const char *textfile, const char *patternfile)
{
    double bleaf = st.ls.nxt_ind * 8;
    double binner = st.is.nxt_ind * 8;
    double txt = text.len * 4;
    double chars = MAX_CHARS * 4;
    double tot = (bleaf + binner + txt + chars);
    fprintf(out, "%s\n%s\n", textfile, patternfile);
    printtime(out);
    fprintf(out, "st.ls.fst         %.2f Mb\n", MEGABYTES(bleaf));
    fprintf(out, "st.is.fst         %.2f Mb\n", MEGABYTES(binner));
    fprintf(out, "text.fst          %.2f Mb\n", MEGABYTES(txt));
    fprintf(out, "text.cs           %.2f Mb\n", MEGABYTES(chars));
    fprintf(out, "filsize:          %.2f Mb\n", (double) MEGABYTES(text.len));
    fprintf(out, "Tot alloc:        %.2f Mb\n", MEGABYTES(tot));
    fprintf(out, "Space/byte:       %.2f\n\n", tot / text.len);
}

static bool search_pattern(STree *st, Wchar *start, Loc *loc)
{
    Uint pattlen = wcslen(start);
    Pattern patt;
    patt.start = start;
    patt.end = start + pattlen;
    Wchar *rem = scan(st, loc, ROOT, patt);
    return !rem || rem[0] == 0;
}

char *log_runtime(FILE *out, char *textfile, char *patternfile)
{
    file_to_string(textfile);
    Wchar **patterns = (Wchar **) malloc(sizeof(Wchar *) * MAX_PATTERNS);
    int npatterns  = file_to_strings(patternfile, MAX_PATTERNS, &patterns);
    STree st;

    initclock();
    construct(&st);

    Loc loc;

    for (Uint j = 0; j < (Uint) npatterns; j++) {

        Wchar *current_pattern = patterns[j];
        search_pattern(&st, current_pattern, &loc);

    }
    print_statistics(out, st, textfile, patternfile);

    for (int i = npatterns; i >= 0; i--) {
        free(patterns[i]);
    }

    free(patterns);
    destroy(&st);
    free(text.fst);
    return NULL;
}


char *test_experiments()
{
    setlocale(LC_ALL, "en_US.utf8");

    char *tfiles[10];
    char *pfiles[10];

    pfiles[0] = "/home/lsund/Data/testdata/members/patterns-diffsize/p20.txt";

    tfiles[0] = "/home/lsund/Data/testdata/members/diffsize/001.txt";
    tfiles[1] = "/home/lsund/Data/testdata/members/diffsize/002.txt";
    tfiles[2] = "/home/lsund/Data/testdata/members/diffsize/004.txt";
    tfiles[3] = "/home/lsund/Data/testdata/members/diffsize/008.txt";
    tfiles[4] = "/home/lsund/Data/testdata/members/diffsize/016.txt";
    tfiles[5] = "/home/lsund/Data/testdata/members/diffsize/032.txt";
    tfiles[6] = "/home/lsund/Data/testdata/members/diffsize/064.txt";
    tfiles[7] = "/home/lsund/Data/testdata/members/diffsize/128.txt";
    /* tfiles[8] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */
    /* tfiles[9] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */

    const char *outfile = "data/experiments.txt";
    FILE *out = fopen(outfile, "w");
    for (int i = 0; i < 6; i++) {
        log_runtime(out, tfiles[i], pfiles[0]);
    }
    fclose(out);

    return NULL;
}

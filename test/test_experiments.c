#include "test.h"

static bool search_pattern(STree *st, Wchar *start, Loc *loc)
{
    Uint pattlen = wcslen(start);
    Pattern patt;
    patt.start = start;
    patt.end = start + pattlen;
    Wchar *rem = scan(st, loc, ROOT, patt);
    return !rem || rem[0] == 0;
}

char *log_runtime(char *textfile, char *patternfile)
{
    int maxpatterns = 200;

    Uint patternslen;
    file_to_string(textfile);
    Wchar **patterns = (Wchar **) malloc(sizeof(Wchar *) * MAX_PATTERNS);
    int npatterns  = file_to_strings(patternfile, &patternslen, MAX_PATTERNS, &patterns);
    STree st;

    initclock();
    construct(&st);

    Loc loc;

    for (Uint j = 0; j < min(npatterns, maxpatterns); j++) {

        Wchar *current_pattern = patterns[j];
        search_pattern(&st, current_pattern, &loc);

    }
    printf("%lu\n", text.len);
    printtime(stdout);

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
    /* tfiles[6] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */
    /* tfiles[7] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */
    /* tfiles[8] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */
    /* tfiles[9] = "/home/lsund/Data/testdata/members/diffsize/001.txt"; */

    for (int i = 0; i < 5; i++) {
        log_runtime(tfiles[i], pfiles[0]);
    }
    /* log_runtime(tfiles[0], pfiles[0]); */

    return NULL;
}

#include "test.h"

size_t strlenw(Wchar *s)
{
    size_t i = 0;
    while (s[i]) {
        i++;
    }
    return i;
}


static bool search_pattern(STree *st, Wchar *start, Loc *loc)
{
    Uint pattlen = strlenw(start);
    Pattern patt;
    patt.start = start;
    patt.end = start + pattlen;
    Wchar *rem = scan(st, loc, ROOT, patt);
    return !rem || rem[0] == 0;
}

char *test_count(char *patternfile, char *textfile, Uint count)
{
    Uint patternslen;
    setlocale(LC_ALL, "en_US.utf8");
    file_to_string(textfile);
    Wchar **patterns = (Wchar **) malloc(sizeof(Wchar *) * MAX_PATTERNS);
    int npatterns  = file_to_strings(patternfile, &patternslen, MAX_PATTERNS, &patterns);
    STree st;
    construct(&st);

    Loc loc;

    Uint exists_n = 0;
    for (Uint j = 0; j < (Uint) npatterns; j++) {

        Wchar *current_pattern = patterns[j];

        bool exists = search_pattern(&st, current_pattern, &loc);

        exists ? exists_n++ : (void) 0;
    }
    for (int i = npatterns - 1; i >= 0; i--) {
        free(patterns[i]);
    }

    free(patterns);

    mu_assert("Count should be correct", exists_n == count);

    return NULL;
}

char *compare_vs_naive(char *patternfile, char *textfile)
{
    int maxpatterns = 200;

    Uint patternslen;
    setlocale(LC_ALL, "en_US.utf8");
    file_to_string(textfile);
    Wchar **patterns = (Wchar **) malloc(sizeof(Wchar *) * MAX_PATTERNS);
    int npatterns  = file_to_strings(patternfile, &patternslen, MAX_PATTERNS, &patterns);
    STree st;
    construct(&st);

    Loc loc;

    int exists_n = 0, rexists_n = 0;
    for (Uint j = 0; j < min(npatterns, maxpatterns); j++) {

        Wchar *current_pattern = patterns[j];
        Uint patternlen = strlenw(current_pattern);

        Wchar *end = current_pattern + patternlen;

        bool exists = search_pattern(&st, current_pattern, &loc);
        bool rexists = naive_search(current_pattern, end);

        exists ? exists_n++ : (void) 0;
        rexists ? rexists_n++ : (void) 0;

        if (rexists != exists) {
            printf("%d %d\n", rexists, exists);
            printf("Fail on: %ls\n", patterns[j]);
        }
        mu_assert(
            "Naive and suffix tree sourch should be the same.",
            rexists == exists
        );
    }
    for (int i = npatterns - 1; i >= 0; i--) {
        free(patterns[i]);
    }
    free(patterns);
    return NULL;
}

char *leafcounts(const char *fname)
{
    setlocale(LC_ALL, "en_US.utf8");
    file_to_string(fname);
    STree st;
    construct(&st);

    Wchar current_pattern[5] = L"211";

    Loc loc;
    bool exists = search_pattern(&st, current_pattern, &loc);
    printf("exists: %d\n", exists);

    Reference start;
    start.toleaf = loc.leafedge;
    start.address = loc.nxt;
    ArrayUint stack;
    stack.spaceUint = NULL;
    stack.allocatedUint = stack.nxtfreeUint = 0;

    makeleaflist(&st, &stack, &start);

    return NULL;
}

char *utest_leaves()
{
    char *error;
    mu_message(DATA, "Leafs: Smyth\n");
    error = leafcounts("data/smyth.txt");
    if (error) return error;

    return NULL;
}

char *utest_patterns()
{


    char *error;
    mu_message(DATA, "Trivial\n");
    error = compare_vs_naive(
                "data/trivial-patterns.txt",
                "data/trivial.txt"
            );
    if (error) return error;


    mu_message(DATA, "Random existing patterns\n");
    error = compare_vs_naive(
                "/home/lsund/Data/testdata/members/random-patterns.txt",
                "/home/lsund/Data/testdata/members/diffsize/005.txt"
            );
    if (error) return error;

    mu_message(DATA, "Random non-existing patterns\n");
    error = compare_vs_naive(
                "/home/lsund/Data/testdata/members/random-patterns-non-existing.txt",
                "/home/lsund/Data/testdata/members/diffsize/005.txt"
            );
    if (error) return error;

    mu_message(DATA, "Akz patterns\n");
    error = compare_vs_naive(
                "/home/lsund/Data/testdata/akz/10000.txt",
                "/home/lsund/Data/testdata/akz/data.xml"
            );
    if (error) return error;

    mu_message(DATA, "Akz patterns, take 2\n");
    error = compare_vs_naive(
                "/home/lsund/Data/testdata/doctronic/diffsize/12000.txt",
                "/home/lsund/Data/testdata/doctronic/data-diffsize/small.xml");
    if (error) return error;

    mu_message(DATA, "Count: Akz patterns\n");
    error = test_count(
                "/home/lsund/Data/testdata/akz/10000.txt",
                "/home/lsund/Data/testdata/akz/data.xml",
                174
            );
    if (error) return error;

    mu_message(DATA, "Count: Akz patterns, take 2\n");
    error = test_count(
                "/home/lsund/Data/testdata/doctronic/diffsize/12000.txt",
                "/home/lsund/Data/testdata/doctronic/data-diffsize/small.xml",
                170
            );
    if (error) return error;

    return NULL;
}

char *test_search()
{
    mu_run_utest(utest_patterns);
    mu_run_utest(utest_leaves);

    return NULL;
}


#include "construct.h"
#include <locale.h>
#include "io.h"


int main(int argc, char *argv[])
{
    STree st;

    if (argc != 2) {
        fprintf(stderr, "Need exactly one argument");
        return EXIT_FAILURE;
    }
    char *textfile = argv[1];

    setlocale(LC_ALL, LOCALE);
    printf("Loading a text file based on the locale: %s\n", LOCALE);
    text_initialize(textfile);

    clock_init();

    fprintf(stdout, "Creating a suffix tree for text based on %s\n", textfile);
    construct(&st);

    stree_destroy(&st);
    text_destroy();

    fprintf(stdout, "Done in %.2f seconds\n", getruntime());

    return EXIT_SUCCESS;
}

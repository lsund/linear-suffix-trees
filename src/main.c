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
    char *filename = argv[1];

    setlocale(LC_ALL, "en_US.utf8");
    file_to_string(filename);

    initclock();

    fprintf(stdout, "Creating a suffix tree for text of length %lu\n", text.len);

    construct(&st);

    return EXIT_SUCCESS;
}

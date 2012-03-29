#include "libpar2.h"

void usage(void)
{
    fprintf(stderr, "libpar2_sample [filename or -]\n");
}

int invalid_args(int argc, char *argv[])
{
    int invalid = 1;

    if (argc == 2) {
        invalid = 0;
    }

    return invalid;
}

void view_arg(int argc, char *argv[])
{
    int i;

    for (i=0;i<argc;i++) {
        fprintf(stderr, "argv[%d] = \"%s\"\n", i, argv[i]);
    }
}

int main(int argc, char *argv[])
{
    int i;

    if (invalid_args(argc, argv)) {
        view_arg(argc, argv);
        usage();
        return -1;
    }

    return 0;
}

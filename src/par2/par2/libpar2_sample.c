#include "libpar2.h"

typedef struct _opts_t {
    int encode;
    int decode;
} opts_t;

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

void view_opts_t(opts_t *opts)
{
    fprintf(stderr, "encode = %d\n", opts->encode);
    fprintf(stderr, "decode = %d\n", opts->decode);
}

int parse_arg(opts_t *opts, int argc, char *argv[])
{
    int i;

    memset(opts, 0, sizeof(opts_t));

    /* check encode or decode. */
    for (i=0;i<argc;i++) {
        if (strcmp("--encode", argv[i]) == 0)
            opts->encode = 1;
        else if (strcmp("--decode", argv[i]) == 0)
            opts->decode = 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    opts_t opts;

    parse_arg(&opts, argc, argv);
    if (invalid_args(argc, argv)) {
        view_arg(argc, argv);
        usage();
        return -1;
    }
    else {
        view_opts_t(&opts);
    }

    return 0;
}

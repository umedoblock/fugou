#include "libpar2.h"

#define ENABLE 1

typedef struct _opts_t {
    int encode;
    int decode;
} opts_t;

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

void usage(void)
{
    fprintf(stderr, "libpar2_sample [filename or -]\n");
}

int invalid_opts(opts_t *opts)
{
    int invalid = 1;

    if (opts->encode == ENABLE ||
        opts->decode == ENABLE) {
        invalid = 0;
    }

err:
    return invalid;
}

int parse_arg(opts_t *opts, int argc, char *argv[])
{
    int i;

    memset(opts, 0, sizeof(opts_t));

    /* check encode or decode. */
    for (i=0;i<argc;i++) {
        if (strcmp("--encode", argv[i]) == 0)
            opts->encode = ENABLE;
        if (strcmp("--decode", argv[i]) == 0)
            opts->decode = ENABLE;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    opts_t opts;

    parse_arg(&opts, argc, argv);
    if (invalid_opts(&opts)) {
        view_arg(argc, argv);
        usage();
        return -1;
    }
    view_opts_t(&opts);

    return 0;
}

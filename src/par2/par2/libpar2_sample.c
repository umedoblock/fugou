#include "libpar2.h"

#define ENABLE 1

typedef struct _opts_t {
    int encode;
    int decode;
    int redundancy;
    int bits;
    char *file;
    FILE *fp;
} opts_t;

void view_args(int argc, char *argv[])
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
    fprintf(stderr, "redundancy = %d\n", opts->redundancy);
    fprintf(stderr, "bits = %d\n", opts->bits);
    fprintf(stderr, "file = \"%s\"\n", opts->file);
    fprintf(stderr, "fp = %p\n", opts->fp);
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
    }
    else {
        fprintf(stderr, "have to use --encode or --decode.\n");
        goto err;
    }
    if (opts->encode == ENABLE) {
        if (opts->decode == ENABLE) {
            fprintf(stderr, "same time use --encode and --decode option.\n");
            fprintf(stderr, "Therefore canceled --decode option.\n");
            opts->decode = 0;
        }
        if (opts->redundancy <= 0 ||
            opts->bits <= 0) {
            fprintf(stderr, "when use --encode option,\n");
            fprintf(stderr, "NUM must be positive number.\n");
            fprintf(stderr, "have to use --redundancy=NUM and --bits=NUM.\n");
            goto err;
        }
    }
    if (opts->decode == ENABLE) {
        if (opts->redundancy != 0 ||
            opts->bits != 0)
            fprintf(stderr, "when use --decode option.\n");
        if (opts->redundancy != 0)
            fprintf(stderr, "no need to use --redundancy option.\n");
        if (opts->bits != 0)
            fprintf(stderr, "no need to use --bits option.\n");
    }

    invalid = 0;
err:
    return invalid;
}

int parse_encode_or_decode(opts_t *opts, int argc, char *argv[])
{
    int i;

    /* check encode or decode. */
    for (i=0;i<argc;i++) {
        if (strcmp("--encode", argv[i]) == 0)
            opts->encode = ENABLE;
        if (strcmp("--decode", argv[i]) == 0)
            opts->decode = ENABLE;
    }

    return 0;
}

int parse_numeric_arg(
    opts_t *opts, char *opt_name, int argc, char *argv[])
{
    int i;
    int num = 0, opt_name_len = -1, argv_len = -1;

    opt_name_len = strlen(opt_name);
    for (i=0;i<argc;i++){
        /* expect opt_name style
         * opt_name=NUM
         */
        argv_len = strlen(argv[i]);
        if (argv_len > opt_name_len + 1 &&
            strncmp(opt_name, argv[i], opt_name_len) == 0 &&
            argv[i][opt_name_len] == '=') {
            num = atoi(argv[i] + opt_name_len + 1);
            break;
        }
    }

    return num;
}

char *parse_string_arg(
    opts_t *opts, char *opt_name, int argc, char *argv[])
{
    int i;
    int opt_name_len = -1, argv_len = -1;
    char *p = NULL;

    opt_name_len = strlen(opt_name);
    for (i=0;i<argc;i++){
        /* expect opt_name style
         * opt_name=NUM
         */
        argv_len = strlen(argv[i]);
        if (argv_len > opt_name_len + 1 &&
            strncmp(opt_name, argv[i], opt_name_len) == 0 &&
            argv[i][opt_name_len] == '=') {
            p = argv[i] + opt_name_len + 1;
            break;
        }
    }

    return p;
}

int parse_redundancy_and_bits(opts_t *opts, int argc, char *argv[])
{
    opts->redundancy = parse_numeric_arg(opts, "--redundancy", argc, argv);
    opts->bits = parse_numeric_arg(opts, "--bits", argc, argv);

    return 0;
}

int parse_file(opts_t *opts, int argc, char *argv[])
{
    opts->file = parse_string_arg(opts, "--file", argc, argv);

    return 0;
}

int parse_args(opts_t *opts, int argc, char *argv[])
{
    int i;

    memset(opts, 0, sizeof(opts_t));

    parse_encode_or_decode(opts, argc, argv);
    parse_redundancy_and_bits(opts, argc, argv);
    parse_file(opts, argc, argv);

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    opts_t opts;

    parse_args(&opts, argc, argv);
    if (invalid_opts(&opts)) {
        view_args(argc, argv);
        usage();
        return -1;
    }
    view_opts_t(&opts);

    return 0;
}

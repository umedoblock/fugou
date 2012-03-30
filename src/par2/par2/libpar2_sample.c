#include "libpar2.h"

#define ENABLE 1
#define MAX_REDUNDANCY 8192

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
    fprintf(stderr, "view_opts_t(opts=%p)\n", opts);
    fprintf(stderr, "encode = %d\n", opts->encode);
    fprintf(stderr, "decode = %d\n", opts->decode);
    fprintf(stderr, "redundancy = %d\n", opts->redundancy);
    fprintf(stderr, "bits = %d\n", opts->bits);
    fprintf(stderr, "file = \"%s\"\n", opts->file);
    fprintf(stderr, "fp = %p\n", opts->fp);
}

void usage(void)
{
    fprintf(stderr, "libpar2_sample --encode " \
                    "--redundancy=NUM --bits=NUM --file=NAME\n");
    fprintf(stderr, "or\n");
    fprintf(stderr, "libpar2_sample --decode --file=NAME\n");
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
        if (opts->redundancy <= 1 ||
            opts->bits <= 0) {
            fprintf(stderr, "when use --encode option,\n");
            fprintf(stderr, "have to use --redundancy >= 2 and " \
                            "--bits=(4, 8, 16 or 24).\n");
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

int parse_string_arg_about_existential(
    opts_t *opts, char *opt_name, int argc, char *argv[])
{
    int i, existence = 0;
    int opt_name_len = -1, argv_len = -1;

    opt_name_len = strlen(opt_name);
    for (i=0;i<argc;i++){
        /* expect opt_name style
         * opt_name
         */
        argv_len = strlen(argv[i]);
        if (strcmp(opt_name, argv[i]) == 0) {
            existence = 1;
            break;
        }
    }

    return existence;
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
    int help;

    help = parse_string_arg_about_existential(opts, "--help", argc, argv);
    if (help)
        return 1;
    help = parse_string_arg_about_existential(opts, "-h", argc, argv);
    if (help)
        return 1;

    memset(opts, 0, sizeof(opts_t));

    parse_encode_or_decode(opts, argc, argv);
    parse_redundancy_and_bits(opts, argc, argv);
    parse_file(opts, argc, argv);

    return 0;
}

int open_file(opts_t *opts)
{
    char *mode = NULL;

    if (opts->encode == ENABLE)
        mode = "rb";
    else if (opts->decode == ENABLE)
        mode = "wb";
    else
        return -1;

    opts->fp = fopen(opts->file, mode);
    if (opts->fp != NULL) {
        fprintf(stderr, "opened \"%s\" with fp = %p and mode \"%s\"\n",
                                opts->file, opts->fp, mode);
    }
    else {
        fprintf(stderr, "cannot open \"%s\" with fp = %p and mode \"%s\"\n",
                                opts->file, opts->fp, mode);
    }

    return 0;
}

int close_file(opts_t *opts)
{
    if (opts->fp != NULL) {
        fprintf(stderr, "closed \"%s\" with fp = %p\n",
                                opts->file, opts->fp);
        fclose(opts->fp);
    }
    else {
        fprintf(stderr, "cannot close \"%s\" with fp = %p\n",
                                opts->file, opts->fp);
    }

    return 0;
}

int sample_init_p2(par2_t *p2, int redundancy, int bits)
{
    int ret, max_redundancy;

    ret = par2_init_p2(p2, redundancy, bits);
    if (ret == 0) {
        if (bits == 16 || bits == 24) {
            if (redundancy > MAX_REDUNDANCY)
                ret = PAR2_INVALID_REDUNDANCY_ERROR;
        }
    }
    if (ret < 0){
        if (ret == PAR2_INVALID_BITS_ERROR)
            fprintf(stderr, "must chose 4, 8, 16 or 24 for bits.\n");
        else if (ret == PAR2_INVALID_REDUNDANCY_ERROR) {
            if (bits == 4 || bits == 8) {
                max_redundancy = p2->rds->gf_max;
            }
            else {
                /* bits == 16 || bits == 24 */
                max_redundancy = MAX_REDUNDANCY;
            }
            fprintf(stderr, "redundancy(=%d) must be 2 <= redundancy <= %d.\n",
                             redundancy, max_redundancy);
        }
        else
            fprintf(stderr, "unknown return code that is %d.\n", ret);
        par2_view_p2(p2);

        #define OHHHHHHHHHHHHHHHHHHHH_NOOOOOOOOOOOOO -2
        ret = OHHHHHHHHHHHHHHHHHHHH_NOOOOOOOOOOOOO;
    }

    return ret;
}

#define SEE_YOU 0

int main(int argc, char *argv[])
{
    int i, help = 0, done, ret;
    int redundancy, bits;
    opts_t opts;
    /* please see par2/pypar2.c Par2_init() in detail. */
    /* need p2 for libpar2. */
    par2_t par2, *p2 = NULL;

    help = parse_args(&opts, argc, argv);
    if (invalid_opts(&opts) || help) {
        view_args(argc, argv);
        usage();
        return -1;
    }
    /* view_opts_t(&opts); */

    open_file(&opts);

    /* view_opts_t(&opts); */

    /* MAIN ROUTINE for libpar2. */
    /* below three variant need to use libpar2. */
    redundancy = opts.redundancy;
    bits = opts.bits;
    p2 = &par2;

    /* occured par2 big bang !!! */
    done = par2_big_bang();
    if (done == PAR2_MALLOC_ERROR){
        fprintf(stderr, "par2_big_bang() failed for memory.\n");
        return PAR2_MALLOC_ERROR;
    }

    /* you can call par2_init_p2() */
    ret = sample_init_p2(p2, redundancy, bits);
    if (ret < 0)
        return ret;

    /* black hole appered. */
    par2_ultimate_fate_of_the_universe();

    close_file(&opts);

    /* THANK YOU. */
    return SEE_YOU;
}

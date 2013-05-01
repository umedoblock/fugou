#include "libpar2.h"

#define ENABLE 1
#define MAX_REDUNDANCY 8192

typedef struct _opts_t {
    int encode;
    int decode;
    int division;
    int bits;
    char *path;
    char *header;
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
    fprintf(stderr, "division = %d\n", opts->division);
    fprintf(stderr, "bits = %d\n", opts->bits);
}

void usage(void)
{
    fprintf(stderr, "libpar2_sample --encode " \
                    "--division=NUM --bits=NUM --file=PATH\n");
    fprintf(stderr, "or\n");
    fprintf(stderr, "libpar2_sample --decode --header=PATH\n");
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
        if (opts->division <= 1 ||
            opts->bits <= 0) {
            fprintf(stderr, "when use --encode option,\n");
            fprintf(stderr, "have to use --division >= 2 and " \
                            "--bits=(4, 8, 16 or 24).\n");
            goto err;
        }
    }
    if (opts->decode == ENABLE) {
        if (opts->division != 0 ||
            opts->bits != 0)
            fprintf(stderr, "when use --decode option.\n");
        if (opts->division != 0)
            fprintf(stderr, "no need to use --division option.\n");
        if (opts->bits != 0)
            fprintf(stderr, "no need to use --bits option.\n");
        if (opts->header == NULL) {
            fprintf(stderr, "must use --header option.\n");
            goto err;
        }
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

    for (i=0;i<argc;i++){
        if (strcmp(opt_name, argv[i]) == 0) {
            existence = 1;
            break;
        }
    }

    return existence;
}

int parse_division_and_bits(opts_t *opts, int argc, char *argv[])
{
    opts->division = parse_numeric_arg(opts, "--division", argc, argv);
    opts->bits = parse_numeric_arg(opts, "--bits", argc, argv);

    return 0;
}

int parse_file(opts_t *opts, int argc, char *argv[])
{
    opts->path = parse_string_arg(opts, "--file", argc, argv);

    return 0;
}

int parse_header(opts_t *opts, int argc, char *argv[])
{
    opts->header = parse_string_arg(opts, "--header", argc, argv);

    return 0;
}

int parse_args(opts_t *opts, int argc, char *argv[])
{
    int help;

    help = parse_string_arg_about_existential(opts, "--help", argc, argv);
    if (help)
        return 1;
    help = parse_string_arg_about_existential(opts, "-h", argc, argv);
    if (help)
        return 1;

    memset(opts, 0, sizeof(opts_t));

    parse_encode_or_decode(opts, argc, argv);
    parse_division_and_bits(opts, argc, argv);
    parse_file(opts, argc, argv);
    parse_header(opts, argc, argv);

    return 0;
}

#if 0
int open_file(par2_file_t *p2f, opts_t *opts)
{
    char *mode = NULL;

    p2f->fp = NULL;
    p2f->data = NULL;

    if (opts->encode == ENABLE)
        mode = "rb";
    else if (opts->decode == ENABLE)
        mode = "r";
    else
        return -1;

    p2f->fp = fopen(p2f->name, mode);
    if (p2f->fp != NULL) {
        fprintf(stderr, "opened \"%s\" with fp = %p and mode \"%s\"\n",
                                p2f->name, p2f->fp, mode);
    }
    else {
        fprintf(stderr, "cannot open \"%s\" with fp = %p and mode \"%s\"\n",
                                p2f->name, p2f->fp, mode);
        return -2;
    }

    return 0;
}

int read_file(par2_file_t *p2f)
{
    fseek(p2f->fp, 0L, SEEK_END);
    p2f->file_size = ftell(p2f->fp);
    fseek(p2f->fp, 0L, SEEK_SET);
    p2f->data = (uchar *)malloc(p2f->file_size);
    if (p2f->data == NULL) {
        fprintf(stderr, "\n");
        return -4;
    }
    fread(p2f->data, p2f->file_size, 1, p2f->fp);

    return 0;
}

int free_file(par2_file_t *p2f)
{
    if (p2f->data != NULL) {
        fprintf(stderr, "freed \"%s\" with data = %p\n",
                                p2f->name, p2f->data);
        free(p2f->data);
        p2f->data = NULL;
    }
    else {
        fprintf(stderr, "cannot free \"%s\" with data = %p\n",
                                p2f->name, p2f->data);
    }

    return 0;
}

int close_file(par2_file_t *p2f)
{
    free_file(p2f);
    if (p2f->fp != NULL) {
        fprintf(stderr, "closed \"%s\" with fp = %p\n",
                                p2f->name, p2f->fp);
        fclose(p2f->fp);
        p2f->fp = NULL;
    }
    else {
        fprintf(stderr, "cannot close \"%s\" with fp = %p\n",
                                p2f->name, p2f->fp);
    }

    return 0;
}

void *allocate_resource(
    FILE ***files, char ***names, int names_num)
{
    size_t mem_size;
    void *mem, *mem_;
    char **names_;
    int i;

    /* header + data * division + parity * division */
    mem_size = 0;
    mem_size += sizeof(FILE *) * names_num;
    mem_size += sizeof(char *) * names_num;
    mem_size += L_tmpnam * names_num;

    mem = (void *)malloc(mem_size);
    if (mem == NULL) {
        fprintf(stderr, "cannot allocate memory(=%d)\n", mem_size);
        return NULL;
    }
    mem_ = mem;
    *files = mem; mem += sizeof(FILE *) * names_num;
    *names = mem; mem += sizeof(char *) * names_num;
    names_ = *names;
    for (i=0;i<names_num;i++) {
        names_[i] = mem;
        mem += L_tmpnam;
    }

    return mem_;
}

int open_par2_files(char **names, FILE **files, int names_num)
{
    int i;
    FILE *fp;
    for (i=0;i<names_num;i++) {
        tmpnam(names[i]);
        fp = fopen(names[i], "wb");
        files[i] = fp;
        fprintf(stdout, "names[%d] = %s, fp = %p\n", i, names[i], fp);
    }
    return 0;
}

int close_par2_files(char **names, FILE **files, int names_num)
{
    int i, ret;
    FILE *fp;

    for (i=0;i<names_num;i++){
        fp = files[i];
        fclose(fp);
        ret = remove(names[i]);
        fprintf(stdout, "remove(names[%d]=%s)=%d, fclose(%p)\n", \
                                      i, names[i], ret, fp);
    }
    return 0;
}
#endif

#define SEE_YOU 0

int main(int argc, char *argv[])
{
    int help = 0, ret = -1;
    uint division, bits;
    opts_t opts_, *opts = &opts_;
    char hashed_header[80], ss[80];
    /* need p2e for libpar2. */

    memset(opts, 0, sizeof(opts_t));

    help = parse_args(opts, argc, argv);
    if (invalid_opts(opts) || help) {
        view_args(argc, argv);
        usage();
        return -100;
    }
    /* view_opts_t(opts); */

    /* MAIN ROUTINE for libpar2. */
    /* below three variant need to use libpar2. */
    if (opts->encode == ENABLE) {
        division = opts->division;
        bits = opts->bits;
    }
    else if (opts->decode == ENABLE) {
    }
    else {
        return -201;
    }

    /* occured par2 big bang !!! */
    ret = par2_big_bang();
    if (ret < 0) {
        fprintf(stderr, "par2_big_bang() failed for memory.\n");
        return ret;
    }
    /*
    fprintf(stdout, "bits = %d, division = %d in main()\n", \
                     bits, division);
    fprintf(stdout, "path = \n");
    fprintf(stdout, "%s\n", opts->path);
    */

    /* please see par2/pypar2.c Par2_init() in detail. */

    if (opts->encode == ENABLE) {
        ret = par2_encode_file(hashed_header, opts->path, bits, division);
        if (ret >= 0) {
            fprintf(stdout, "%s\n", hashed_header);
            ret = SEE_YOU;
        }
        else {
            fprintf(stderr, "failed par2_encode_file() = %d\n", ret);
        }
    }
    else if (opts->decode == ENABLE) {
        ret = par2_recover_file(ss, opts->header);
        if (ret >= 0) {
            fprintf(stdout, "recovered file hash value is =>\n\"%s\"\n", ss);
            ret = SEE_YOU;
        }
        else {
            fprintf(stderr, "failed par2_decode_file() = %d\n", ret);
        }
        /*
        */
    }
    else {
        ret = -200;
    }

    /* black hole appered. */
    par2_ultimate_fate_of_the_universe();

    /* THANK YOU. */
    return ret;
}

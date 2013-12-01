#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define SS_SIZE 80
#define BUF_SIZE (sizeof(uint) * 2)

#ifndef FILE_SIZE
#define FILE_SIZE 256
#endif

#define _f stdout

void dump(void *ptr, int length, int width)
{
    int i;
    unsigned char *data = (unsigned char *)ptr;
    char fmt[SS_SIZE];

    sprintf(fmt, "%%p ");
    /*
    fprintf(_f, "dump(ptr=%p, length=%d, width=%d) start\n",
                          ptr, length, width);

    fprintf(_f, "%*s", 3 + sizeof(void *) * 2, "");
    for (i=0;i<width;i++){
        fprintf(_f, "%02x ", (unsigned int )(data + i) & 0x0f);
        if ((i + 1) % 4 == 0) {
            fprintf(_f, "| ");
        }
    }
    if (((i + 1) % width) != 0)
        fprintf(_f, "\n");
    */

    for (i=0;i<length;i++) {
        if (i % width == 0) {
            fprintf(_f, fmt, data + i);
        }
        fprintf(_f, "%02x ", data[i]);
        if ((i + 1) % 4 == 0) {
            fprintf(_f, "| ");
        }
        if ((i + 1) % width == 0) {
            fprintf(_f, "\n");
        }
    }
    if (((length) % width) != 0)
        fprintf(_f, "\n");
}

int main(void)
{
    char random_path[SS_SIZE];
    FILE *fp;
    uchar buf[BUF_SIZE], mem[FILE_SIZE];
    fpos_t fpos[2];
    int i, j, sw;

    sprintf(random_path, "./reed_solomon/random_%u.bin", FILE_SIZE);
    fp = fopen(random_path, "rb");
    fprintf(_f, "random_path = \"%s\", fp = %p\n", random_path, fp);
    if (fp == NULL) {
        fprintf(stderr, "not found \"%s\"\n", random_path);
        return -1;
    }
    fread(mem, 1, FILE_SIZE, fp);
    fprintf(_f, "mem =\n");
    dump(mem, FILE_SIZE, 8);
    fprintf(_f, "\n");

    fseek(fp, 0L, SEEK_SET);
    fgetpos(fp, fpos + 0);

    fseek(fp, 0L + FILE_SIZE / 2, SEEK_SET);
    fgetpos(fp, fpos + 1);

    for (i=0;i<FILE_SIZE/BUF_SIZE;i++) {
        sw = i % 2;
        fsetpos(fp, fpos + sw);
        fread(buf, 1, 8, fp);
        dump(buf, 8, 8);
        fgetpos(fp, fpos + sw);
        if (sw == 1)
            fprintf(_f, "\n");
    }

    fprintf(_f, "sizeof(fpos_t) = %u\n", sizeof(fpos_t));
    fprintf(_f, "sizeof(uint) = %u\n", sizeof(uint));
    fprintf(_f, "sizeof(int) = %u\n", sizeof(int));
    /*
    sizeof(fpos_t) = 12
    */
    fclose(fp);

    sprintf(random_path, "./reed_solomon/random_%u.bin.jump", FILE_SIZE);
    fp = fopen(random_path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "cannot make wb mode file \"%s\".\n", random_path);
        return -1;
    }
    fprintf(stdout, "fopen(random_path=\"%s\", \"wb\")=%p;\n", random_path, fp);
    fseek(fp, 0L + FILE_SIZE, SEEK_SET);
    fprintf(fp, "z");
    fseek(fp, FILE_SIZE - 2, SEEK_CUR);
    /* 効果なし
    fprintf(fp, "\0");
    fprintf(fp, "\x00");
    効果あり
    fprintf(fp, "b");
    fputc(0x00, fp);
    fprintf(fp, "%c", 0x00);
    */
    fprintf(fp, "%c", 0x00);
    fclose(fp);

    sprintf(random_path, "./reed_solomon/random_%u.bin.jump.2", FILE_SIZE);
    fp = fopen(random_path, "wb");
    if (fp == NULL) {
        fprintf(stderr, "cannot make wb mode file \"%s\".\n", random_path);
        return -1;
    }
    fseek(fp, 11 * 26, SEEK_SET);
    fprintf(stdout, "fopen(random_path=\"%s\", \"wb\")=%p;\n", random_path, fp);
    for (i=0;i<26;i++) {
        if (i != 0)
            fseek(fp, -11 * 2, SEEK_CUR);
        else
            fseek(fp, -11, SEEK_CUR);
        for (j=0;j<10;j++) {
            fprintf(fp, "%c", 'z' - i);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    return 0;
}

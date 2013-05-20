#include "libfugou.h"

void dump(void *ptr, int length, int width)
{
    int i;
    unsigned char *data = (unsigned char *)ptr;
    char fmt[BUFFER_SIZE];

    sprintf(fmt, "0x%%0%dx ", sizeof(void *) * 2);
    fprintf(stderr, "dump() start = %p\n", ptr);

    fprintf(stderr, "%*s", 3 + sizeof(void *) * 2, "");
    for (i=0;i<width;i++){
        fprintf(stderr, "%02x ", (unsigned int )(data + i) & 0x0f);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
    }
    if (((i + 1) % width) != 0)
        fprintf(stderr, "\n");

    for (i=0;i<length;i++) {
        if (i % width == 0) {
            fprintf(stderr, fmt, data + i);
        }
        fprintf(stderr, "%02x ", data[i]);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
        if ((i + 1) % width == 0) {
            fprintf(stderr, "\n");
        }
    }
    if (((length) % width) != 0)
        fprintf(stderr, "\n");
}


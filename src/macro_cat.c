#include <stdio.h>

#define doing(do_) (do_ ## er)
#define do2(do_, name) (do_ ## name)
#define rw(name) (read ## name), (writ ## name)
#define rw2(name) (read ## name, writ ## name)
#define rw3(name) read ## name, writ ## name
#define SLOT_READWRIT(name, init) read ## name = init, writ ## name = init
#define SLOT_READWRITA(name, init) *read ## name = init, *writ ## name = init

int main(void)
{
    int i = 0;
    int SLOT_READWRIT(er, 1000);
    void *buf = NULL, SLOT_READWRITA(_buf, NULL);
/*
    int reader = 1000, writer = 1000;
    void *buf = ((void *)0), *read_buf = ((void *)0), *writ_buf = ((void *)0);
*/

    while (i < 10) {
        i++;
        if (i % 2 == 0)
            continue;
        fprintf(stdout, "i = %d\n", i);
    }

    /*
    fprintf(stdout, "reader = %d\n", reader);
    fprintf(stdout, "writer = %d\n", writer);
    fprintf(stdout, "\n");
    fprintf(stdout, "reader = %d\n", doing(read));
    fprintf(stdout, "writer = %d\n", doing(writ));
    fprintf(stdout, "\n");
    */
    fprintf(stdout, "doing(read) = %d\n", doing(read));
    fprintf(stdout, "doing(writ) = %d\n", doing(writ));
    fprintf(stdout, "\n");
    fprintf(stdout, "do2(read, er) = %d\n", do2(read, er));
    fprintf(stdout, "do2(writ, er) = %d\n", do2(writ, er));
    fprintf(stdout, "\n");
    fprintf(stdout, "rw(er): reader = %d, writer = %d\n", rw(er));
    fprintf(stdout, "rw(er): reader = %d, writer = %d\n", (reader), (writer));
    fprintf(stdout, "\n");
    fprintf(stdout, "rw2(er): reader = %d, writer = %d\n", rw2(er));
    fprintf(stdout, "rw2(er): reader = %d, writer = %d\n", (reader, writer));
    fprintf(stdout, "\n");
    fprintf(stdout, "rw3(er): reader = %d, writer = %d\n", rw3(er));
    fprintf(stdout, "rw3(er): reader = %d, writer = %d\n", reader, writer);
    return 0;
}

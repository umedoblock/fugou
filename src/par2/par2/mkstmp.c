#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
$ ./mkstmp
mkstemp() = "mkstemp-YFR02m", ret = 3

$ ./mkstmp
mkstemp() = "mkstemp-KBjZzy", ret = 3
$ ls -l mkstemp*
-rw------- 1 umetaro umetaro 0 Apr 29 11:45 mkstemp-KBjZzy
-rw------- 1 umetaro umetaro 0 Apr 29 11:45 mkstemp-YFR02m
*/

int main(void)
{
    char ss[80];
    int ret;

    strcpy(ss, "mkstemp-XXXXXX");

    /*
    int mkstemp(char *template);
    */
    ret = mkstemp(ss);
    fprintf(stdout, "mkstemp() = \"%s\", ret = %d\n", ss, ret);

    return 0;
}

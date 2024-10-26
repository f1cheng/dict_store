#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "dict_store.h"

/* 1. change to use parameter passing out.
 * 2. gdb testing via VS code or terminal.
 * 3. exception proper handling.
 * compiple: gcc -o main main.c dict_store.c
 */

int fileRead(char *fname, char **dictStr, int *len)
{
    FILE *fp;
    struct stat st;
    stat(fname, &st);
    fp = fopen(fname, "r");
    *len = st.st_size + 1;
    *dictStr =(char *)malloc(*len);
    fread(*dictStr, 1, st.st_size, fp);
    (*dictStr)[*len - 1] = '\0';
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Please input one parameter.\n");
        return 0;
    }
    char *dictStr;
    int len = 0;
    DictStore store;
    fileRead(argv[1], &dictStr, &len);
    printf("dict str=%s\n", dictStr);

    dictParseRaw(dictStr, len, &store);
    dictStorePrint(&store);
    return 0;
}
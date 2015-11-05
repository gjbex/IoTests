#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

long get_size(char *file_name) {
    FILE *fp;
    long size;
    if ((fp = fopen(file_name, "rb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

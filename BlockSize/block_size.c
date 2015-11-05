#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

int main(int argc, char *argv[]) {
    struct statvfs stat;
    if (argc == 1) {
        errx(EXIT_FAILURE, "expecting mount point");
    }
    statvfs(argv[1], &stat);
    printf("%lu\n", stat.f_bsize);
    return EXIT_SUCCESS;
}

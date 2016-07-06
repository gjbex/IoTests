#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "errors.h"
#include "defs.h"
#include "utils.h"

int writeFile(Params cfg, char **files) {
    int fileNr, fd = -1, status = 0;
    long i, remainder;
    char *buffer = initBuffer(cfg, &status);
    FILE *fp;
    if (status != 0) {
        return status;
    }
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++) {
        if (!(files[fileNr] = (char *) calloc(NAME_LENGTH, sizeof(char)))) {
            fprintf(stderr, "can't allocate files[%d]\n", fileNr);
            return EXIT_FAILURE;
        }
        strncpy(files[fileNr], cfg.path, NAME_LENGTH - 1);
        if ((fd = mkstemp(files[fileNr])) == -1) {
            fprintf(stderr, "can't create file descriptor\n");
            return FILE_CREATE_ERROR;
        }
        if (!(fp = fdopen(fd, "w"))) {
            fprintf(stderr, "can't open file '%s' for writing\n",
                    files[fileNr]);
            return FILE_OPEN_ERROR;
        }
        if (cfg.verbose)
            printf("file '%s' created\n", files[fileNr]);
        for (i = 0; i < cfg.fileSize; i += cfg.bufferSize) {
            fwrite(buffer, sizeof(char), cfg.bufferSize, fp);
            if (ferror(fp)) {
                fprintf(stderr, "write error\n");
                return WRITE_ERROR;
            }
        }
        remainder = cfg.fileSize - i;
        if (remainder > 1) {
            fwrite(buffer, sizeof(char), remainder, fp);
            if (ferror(fp)) {
                fprintf(stderr, "write error\n");
                return WRITE_ERROR;
            }
        }
        fclose(fp);
    }
    free(buffer);
    return EXIT_SUCCESS;
}

int readFile(Params cfg, char **files) {
    int fileNr, status = 0;
    long i;
    char *target, *buffer;
    long total = 0, remainder;
    FILE *fp;
    target = initBuffer(cfg, &status);
    if (status != 0) {
        return status;
    }
    buffer = createBuffer(cfg, &status);
    if (status != 0) {
        return status;
    }
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++) {
        if (!(fp = fopen(files[fileNr], "rb"))) {
            fprintf(stderr, "can't open file '%s' for reading\n",
                    files[fileNr]);
            return FILE_OPEN_ERROR;
        }
        for (i = 0; i < cfg.fileSize; i += cfg.bufferSize) {
            total += fread(buffer, sizeof(char), cfg.bufferSize, fp);
            if (ferror(fp)) {
                fprintf(stderr, "read error\n");
                return READ_ERROR;
            }
            if (strncmp(buffer, target, cfg.bufferSize)) {
                fprintf(stderr, "read not equal to target:\n%s\n%s\n",
                        buffer, target);
                return DIFF_ERROR;
            }
        }
        remainder = cfg.fileSize - i;
        if (remainder > 1) {
            total += fread(buffer, sizeof(char), remainder, fp);
            if (ferror(fp)) {
                fprintf(stderr, "read error\n");
                return READ_ERROR;
            }
            if (strncmp(buffer, target, remainder)) {
                fprintf(stderr, "read not equal to target:\n%s\n%s\n",
                        buffer, target);
                return DIFF_ERROR;
            }
        }
        fclose(fp);
    }
    free(buffer);
    return EXIT_SUCCESS;
}

int randomReadFile(Params cfg, char **files) {
    int fileNr;
    long i;
    FILE *fp;
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++) {
        if (!(fp = fopen(files[fileNr], "rb"))) {
            fprintf(stderr, "can't open file '%s' for reading\n",
                    files[fileNr]);
            return FILE_OPEN_ERROR;
        }
        for (i = 0; i < cfg.randomReads; i++) {
            long pos = ((long) rand())*((long) rand()) %
                ((long) cfg.fileSize);
            fseek(fp, pos, SEEK_SET);
            char c = fgetc(fp);
            if (c == EOF) {
                fprintf(stderr, "error reading at position %ld\n", pos);
                return READ_ERROR;
            }
            char target = 'A' + ((pos % cfg.bufferSize) % 26);
            if (c != target) {
                fprintf(stderr, "read not equal to target: "
                        "'%c' <-> '%c' at position %ld\n",
                        c, target, pos);
                return DIFF_ERROR;
            }
            if (cfg.verbose && i % 1000 == 0) {
                struct timeval timePoint;
                gettimeofday(&timePoint, NULL);
                fprintf(stderr, "file %d: %ld random reeds at %f\n",
                        fileNr, i,
                        timePoint.tv_sec + 1.0e-6*timePoint.tv_usec);
            }
        }
        fclose(fp);
        return EXIT_SUCCESS;
    }
}

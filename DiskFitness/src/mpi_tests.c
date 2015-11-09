#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "errors.h"
#include "defs.h"
#include "utils.h"
#include "mpi_utils.h"

int writeFile(Params cfg, char **files) {
    int rank, size;
    int fileNr, status = 0;
    long i, localFileSize, remainder;
    char *buffer = initBuffer(cfg, &status);
    MPI_File *fp;
    MPI_Offset offset, rbound;
    MPI_Status status;
    if (status != 0) {
        return status;
    }
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    compute_bounds(size, rank, cfg.fileSize, &offset, &ubound);
    localFileSize = ubound - offset + 1;
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++) {
        if (!(files[fileNr] = (char *) calloc(NAME_LENGTH, sizeof(char)))) {
            fprintf(stderr, "can't allocate files[%d]\n", fileNr);
            return EXIT_FAILURE;
        }
        strncpy(files[fileNr], cfg.path, NAME_LENGTH - 1);
        MPI_File_open(MPI_COMM_WORLD, files[fileNr],
                      MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, fp);
        if (fp == NULL) {
            fprintf(stderr, "can't open file '%s'\n", files[fileNr]);
            return EXIT_FAILURE;
        }
        if (cfg.verbose)
            printf("file '%s' created by %d\n", files[fileNr], rank);
        MPI_File_seek(fp, offset, MPI_SEEK_SET);
        for (i = 0; i < localFileSize; i += cfg.bufferSize) {
            MPI_File_write(fp, buffer, cfg.bufferSize, MPI_CHAR, &status);
        }
        MPI_File_close(fp);
    }
    free(buffer);
    return EXIT_SUCCESS;
}

int readFile(Params cfg, char **files) {
    int i, fileNr, status = 0;
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
    int i, fileNr;
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
                fprintf(stderr, "file %d: %d random reeds at %f\n",
                        fileNr, i,
                        timePoint.tv_sec + 1.0e-6*timePoint.tv_usec);
            }
        }
        fclose(fp);
        return EXIT_SUCCESS;
    }
}



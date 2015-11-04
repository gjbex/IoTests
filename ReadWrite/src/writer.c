#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "writer_cl.h"

int write_text(char *file_name, long size);
int write_text_buffered(char *file_name, long size, int buffer_size);
int write_binary(char *file_name, long size);
int write_binary_buffered(char *file_name, long size, int buffer_size);
int validateCL(Params *params) ;

int main(int argc, char *argv[]) {
    Params params;
    initCL(&params);
    parseCL(&params, &argc, &argv);
    validateCL(&params);
    if (params.buffer > 0) {
        if (strncmp(params.mode, "text", 6) == 0) {
            write_text_buffered(params.file, params.size, params.buffer);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            write_binary_buffered(params.file, params.size, params.buffer);
        } else {
            errx(EXIT_FAILURE, "unknown mode '%s' for buffered",
                    params.mode);
        }
    } else {
        if (strncmp(params.mode, "text", 4) == 0) {
            write_text(params.file, params.size);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            write_binary(params.file, params.size);
        } else {
            errx(EXIT_FAILURE, "unknown mode '%s' for unbuffered I/O",
                    params.mode);
        }
    }
    finalizeCL(&params);
    return EXIT_SUCCESS;
}

int write_text(char *file_name, long size) {
    double x = 1.0e-10, delta = 1.0e-10;
    long i;
    FILE *fp;
    if ((fp = fopen(file_name, "w")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    for (i = 0; i < size; i++) {
        fprintf(fp, "%.10e\n", x);
        x += delta;
    }
    fclose(fp);
    return EXIT_SUCCESS;
}

int write_text_buffered(char *file_name, long size, int buffer_size) {
    double x = 1.0e-10, delta = 1.0e-10;
    long i;
    char repr[100];
    char *buffer;
    int buffer_idx = 0;
    FILE *fp;
    if ((buffer = (char *) malloc(buffer_size*sizeof(char))) == NULL) {
        errx(EXIT_FAILURE, "can't allocate buffer of size %d", buffer_size);
    }
    if ((fp = fopen(file_name, "w")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    for (i = 0; i < size; i++) {
        int str_size = sprintf(repr, "%.10e\n", x);
        if (buffer_idx + str_size > buffer_size) {
            fwrite(buffer, sizeof(char), buffer_idx, fp);
            buffer_idx = 0;
        }
        buffer_idx += sprintf(buffer + buffer_idx, "%.10e\n", x);
        x += delta;
    }
    if (buffer_idx > 0) {
        fwrite(buffer, sizeof(char), buffer_idx, fp);
    }
    fclose(fp);
    free(buffer);
    return EXIT_SUCCESS;
}

int write_binary(char *file_name, long size) {
    double x = 1.0e-10, delta = 1.0e-10;
    long i;
    FILE *fp;
    if ((fp = fopen(file_name, "wb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    for (i = 0; i < size; i++) {
        fwrite(&x, sizeof(double), 1, fp);
        x += delta;
    }
    fclose(fp);
    return EXIT_SUCCESS;
}

int write_binary_buffered(char *file_name, long size, int buffer_size) {
    double x = 1.0e-10, delta = 1.0e-10;
    double *buffer;
    long i;
    size_t buffer_bytes = (buffer_size/sizeof(double))*sizeof(double),
           nr_written = 0;
    FILE *fp;
    if (buffer_size % sizeof(double) != 0) {
        warnx("buffer size is not a multiple of double precsion type size");
    }
    buffer_size = buffer_bytes/sizeof(double);
    if ((buffer = (double *) malloc(buffer_bytes)) == NULL) {
        errx(EXIT_FAILURE, "can't allocate buffer of size %d",
                buffer_bytes);
    }
    if ((fp = fopen(file_name, "wb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    for (i = 0; i < size; i++) {
        buffer[i % buffer_size] = x;
        if ((i + 1) % buffer_size == 0) {
            nr_written += fwrite(buffer, sizeof(double), buffer_size, fp);
        }
        x += delta;
    }
    if (nr_written < size) {
        nr_written += fwrite(buffer, sizeof(double), size - nr_written, fp);
    }
    fclose(fp);
    free(buffer);
    return EXIT_SUCCESS;
}

int validateCL(Params *params)  {
    if (params->buffer < 0)
        errx(EXIT_FAILURE, "buffer size must be positive or zero");
    if (params->size <= 0)
        errx(EXIT_FAILURE, "file size must be larger than zero");
}

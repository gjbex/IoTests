#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <hdf5.h>

#include "writer_cl.h"
#include "utils.h"

int write_text(char *file_name, long size);
int write_text_buffered(char *file_name, long size, int buffer_size);
int write_binary(char *file_name, long size);
int write_binary_buffered(char *file_name, long size, int buffer_size);
int write_hdf5(char *file_name, long size);
int write_hdf5_buffered(char *file_name, long size, int buffer_size);
int validateCL(Params *params) ;

int main(int argc, char *argv[]) {
    Params params;
    struct timeval startTime, endTime;
    long total_bytes;
    double total_time;
    initCL(&params);
    parseCL(&params, &argc, &argv);
    validateCL(&params);
    gettimeofday(&startTime, NULL);
    if (params.buffer > 0) {
        if (strncmp(params.mode, "text", 6) == 0) {
            write_text_buffered(params.file, params.size, params.buffer);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            write_binary_buffered(params.file, params.size, params.buffer);
        } else if (strncmp(params.mode, "hdf5", 6) == 0) {
            write_hdf5_buffered(params.file, params.size, params.buffer);
        } else {
            errx(EXIT_FAILURE, "unknown mode '%s' for buffered",
                    params.mode);
        }
    } else {
        if (strncmp(params.mode, "text", 4) == 0) {
            write_text(params.file, params.size);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            write_binary(params.file, params.size);
        } else if (strncmp(params.mode, "hdf5", 6) == 0) {
            write_hdf5(params.file, params.size);
        } else {
            errx(EXIT_FAILURE, "unknown mode '%s' for unbuffered I/O",
                    params.mode);
        }
    }
    gettimeofday(&endTime, NULL);
    total_time =  endTime.tv_sec - startTime.tv_sec +
        (endTime.tv_usec - startTime.tv_usec)*1e-6;
    total_bytes = get_size(params.file);
    printf("%s\t%ld\t%d\t%.6f\n", params.file, total_bytes, params.buffer,
            total_time);
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
    int buffer_idx = 0;
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
        buffer[buffer_idx++] = x;
        if (buffer_size == buffer_idx) {
            nr_written += fwrite(buffer, sizeof(double), buffer_size, fp);
            buffer_idx = 0;
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

int write_hdf5(char *file_name, long size) {
    double x = 1.0e-10, delta = 1.0e-10;
    double *data;
    long i;
    hid_t file_id, dataset_id, dataspace_id;
    hsize_t dataspace_dims[1], offset[1], count[1];
    size_t data_bytes = size*sizeof(double);
    herr_t status;
    if ((data = (double *) malloc(data_bytes)) == NULL) {
        errx(EXIT_FAILURE, "can't allocate data of size %lu",
                size*sizeof(double));
    }
    file_id = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT,
                        H5P_DEFAULT);
    dataspace_dims[0] = size;
    dataspace_id = H5Screate_simple(1, dataspace_dims, NULL);
    dataset_id = H5Dcreate(file_id, "/values", H5T_IEEE_F64LE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT,H5P_DEFAULT);
    for (i = 0; i < size; i++) {
        data[i] = x;
        x += delta;
    }
    H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
             H5P_DEFAULT, data);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);
    free(data);
    return EXIT_SUCCESS;
}

int write_hdf5_buffered(char *file_name, long size, int buffer_size) {
    double x = 1.0e-10, delta = 1.0e-10;
    double *buffer;
    long i;
    size_t buffer_bytes = (buffer_size/sizeof(double))*sizeof(double),
           nr_written = 0;
    int buffer_idx = 0;
    hid_t file_id, dataset_id, dataspace_id, memspace_id;
    hsize_t dataspace_dims[1], offset[1], count[1],
            mem_offset[1], mem_count[1];
    herr_t status;
    if (buffer_size % sizeof(double) != 0) {
        warnx("buffer size is not a multiple of double precsion type size");
    }
    buffer_size = buffer_bytes/sizeof(double);
    if ((buffer = (double *) malloc(buffer_bytes)) == NULL) {
        errx(EXIT_FAILURE, "can't allocate buffer of size %d",
                buffer_bytes);
    }
    file_id = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT,
                        H5P_DEFAULT);
    dataspace_dims[0] = size;
    dataspace_id = H5Screate_simple(1, dataspace_dims, NULL);
    dataset_id = H5Dcreate(file_id, "/values", H5T_IEEE_F64LE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT,H5P_DEFAULT);
    count[0] = buffer_size;
    memspace_id = H5Screate_simple(1, count, NULL);
    mem_offset[0] = 0;
    mem_count[0] = buffer_size;
    H5Sselect_hyperslab(memspace_id, H5S_SELECT_SET, mem_offset, NULL,
                        mem_count, NULL);
    count[0] = buffer_size;
    for (i = 0; i < size; i++) {
        buffer[buffer_idx++] = x;
        if (buffer_size == buffer_idx) {
            offset[0] = i - buffer_size + 1;
            H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, NULL,
                                count, NULL);
            H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, memspace_id,
                     dataspace_id, H5P_DEFAULT, buffer);
            buffer_idx = 0;
        }
        x += delta;
    }
    if (buffer_idx > 0) {
        mem_count[0] = buffer_idx;
        H5Sselect_hyperslab(memspace_id, H5S_SELECT_SET, mem_offset, NULL,
                            mem_count, NULL);
        offset[0] = size - buffer_idx;
        count[0] = buffer_idx;
        H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, NULL,
                            count, NULL);
        H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, memspace_id,
                 dataspace_id, H5P_DEFAULT, buffer);
    }
    H5Dclose(dataset_id);
    H5Sclose(memspace_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);
    free(buffer);
    return EXIT_SUCCESS;
}

int validateCL(Params *params)  {
    if (params->buffer < 0)
        errx(EXIT_FAILURE, "buffer size must be positive or zero");
    if (params->size <= 0)
        errx(EXIT_FAILURE, "file size must be larger than zero");
}

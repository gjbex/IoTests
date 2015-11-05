#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <hdf5.h>

#include "reader_cl.h"
#include "utils.h"

int read_text(char *file_name);
int read_text_buffered(char *file_name, int buffer_size);
int read_binary(char *file_name);
int read_binary_buffered(char *file_name, int buffer_size);
int read_hdf5(char *file_name);
int validateCL(Params *params);

int main(int argc, char *argv[]) {
    Params params;
    struct timeval startTime, endTime;
    double total_time;
    long total_bytes;
    initCL(&params);
    parseCL(&params, &argc, &argv);
    validateCL(&params);
    gettimeofday(&startTime, NULL);
    if (params.buffer > 0) {
        if (strncmp(params.mode, "text", 6) == 0) {
            read_text_buffered(params.file, params.buffer);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            read_binary_buffered(params.file, params.buffer);
        } else {
            errx(EXIT_FAILURE, "unknown mode '%s' for buffered",
                    params.mode);
        }
    } else {
        if (strncmp(params.mode, "text", 4) == 0) {
            read_text(params.file);
        } else if (strncmp(params.mode, "binary", 6) == 0) {
            read_binary(params.file);
        } else if (strncmp(params.mode, "hdf5", 4) == 0) {
            read_hdf5(params.file);
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

int read_text(char *file_name) {
    double x, sum = 0.0;
    long nr_reads = 0;
    FILE *fp;
    if ((fp = fopen(file_name, "r")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    while (fscanf(fp, "%le", &x)) {
        if (feof(fp))
            break;
        sum += x;
        nr_reads++;
    }
    fclose(fp);
    fprintf(stderr, "%ld double precision numbers read, sum = %le\n",
            nr_reads, sum);
    return EXIT_SUCCESS;
}

int read_text_buffered(char *file_name, int buffer_size) {
    double x, sum = 0.0;
    long nr_reads = 0, nr_read;
    char repr[100];
    char *buffer, *str_ptr, *new_str_ptr;
    FILE *fp;
    int offset;
    if ((buffer = (char *) malloc((buffer_size + 1)*sizeof(char))) == NULL)
        errx(EXIT_FAILURE, "can't allocate buffer of size %d", buffer_size);
    buffer[buffer_size] = '\0';
    if ((fp = fopen(file_name, "rb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    str_ptr = buffer;
    offset = 0;
    while (nr_read = fread(str_ptr + offset, sizeof(char),
                           buffer_size - offset, fp)) {
        *(str_ptr + offset + nr_read) = '\0';
        while ((new_str_ptr = strchr(str_ptr, '\n')) != NULL) {
            sscanf(str_ptr, "%le", &x);
            sum += x;
            nr_reads++;
            str_ptr = new_str_ptr + 1;
        }
        if (nr_read < buffer_size - offset)
            break;
        if (strlen(str_ptr) > 0) {
            strcpy(buffer, str_ptr);
        }
        offset = strlen(str_ptr);
        str_ptr = buffer;
    }
    fclose(fp);
    free(buffer);
    fprintf(stderr, "%ld double precision numbers read, sum = %le\n",
            nr_reads, sum);
    return EXIT_SUCCESS;
}

int read_binary(char *file_name) {
    double x, sum = 0.0;
    long nr_reads = 0;
    FILE *fp;
    if ((fp = fopen(file_name, "rb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    while (fread(&x, sizeof(double), 1, fp)) {
        sum += x;
        nr_reads++;
    }
    fclose(fp);
    fprintf(stderr, "%ld double precision numbers read, sum = %le\n",
            nr_reads, sum);
    return EXIT_SUCCESS;
}

int read_binary_buffered(char *file_name, int buffer_size) {
    double x, sum = 0.0;
    double *buffer;
    long nr_read, nr_reads = 0;
    size_t buffer_bytes = (buffer_size/sizeof(double))*sizeof(double);
    FILE *fp;
    int i;
    if (buffer_size % sizeof(double) != 0) {
        warnx("buffer size is not a multiple of double precsion type size");
    }
    buffer_size = buffer_bytes/sizeof(double);
    if ((buffer = (double *) malloc(buffer_bytes)) == NULL) {
        errx(EXIT_FAILURE, "can't allocate buffer of size %d",
                buffer_bytes);
    }
    if ((fp = fopen(file_name, "rb")) == NULL) {
        err(EXIT_FAILURE, "can't open file '%s'", file_name);
    }
    while ((nr_read = fread(buffer, sizeof(double), buffer_size, fp))) {
        for (i = 0; i < nr_read; i++) {
            sum += buffer[i];
        }
        nr_reads += nr_read;
    }
    fclose(fp);
    fprintf(stderr, "%ld double precision numbers read, sum = %le\n",
            nr_reads, sum);
    free(buffer);
    return EXIT_SUCCESS;
}

int read_hdf5(char *file_name) {
    double sum = 0.0;
    double *data;
    long i;
    hid_t file_id, dataset_id, dataspace_id;
    hsize_t dataspace_dims[1];
    size_t data_bytes;
    herr_t status;
    file_id = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);
    dataset_id = H5Dopen(file_id, "/values", H5P_DEFAULT);
    dataspace_id = H5Dget_space(dataset_id);
    H5Sget_simple_extent_dims(dataspace_id, dataspace_dims, NULL);
    data_bytes = dataspace_dims[0]*sizeof(double);
    if ((data = (double *) malloc(data_bytes)) == NULL) {
        errx(EXIT_FAILURE, "can't allocate data of size %d", data_bytes);
    }
    H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
            H5P_DEFAULT, data);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);
    for (i = 0; i < dataspace_dims[0]; i++) {
        sum += data[i];
    }
    fprintf(stderr, "%lu double precision numbers read, sum = %le\n",
            (long unsigned) dataspace_dims[0], sum);
    free(data);
    return EXIT_SUCCESS;
}

int validateCL(Params *params)  {
    if (params->buffer < 0)
        errx(EXIT_FAILURE, "buffer size must be positive or zero");
}

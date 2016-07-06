#include <stdlib.h>
#include <sys/time.h>

#include "utils.h"

char *createBuffer(Params cfg, int *status) {
    char *buffer;
    int i;
    if (!(buffer = (char *) calloc(cfg.bufferSize, sizeof(char)))) {
        fprintf(stderr, "can't allocate buffer\n");
        *status = EXIT_FAILURE;
        return NULL;
    }
    for (i = 0; i < cfg.bufferSize; i++)
        buffer[i] = '\0';
    return buffer;

}

char *initBuffer(Params cfg, int *status) {
    char *buffer = createBuffer(cfg, status);
    char c = 'A';
    int i;
    if (*status != 0) {
        return NULL;
    }
    for (i = 0; i < cfg.bufferSize; i++) {
        buffer[i] = c;
        c = (c < 'Z') ? c + 1 : 'A';
    }
    return buffer;
}

double timeOperation(int (*op)(Params, char **),
                     Params cfg, char **files, int *status) {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    *status = op(cfg, files);
    gettimeofday(&endTime, NULL);
    return endTime.tv_sec - startTime.tv_sec +
        (endTime.tv_usec - startTime.tv_usec)*1e-6;
}

void cleanUp(Params cfg, char **files) {
    int fileNr;
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++)
        remove(files[fileNr]); 
}

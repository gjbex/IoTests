#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

#include "config.h"
#include "defs.h"
#include "errors.h"
#include "tests.h"
#include "utils.h"
#include "mpi_utils.h"

int main(int argc, char *argv[]) {
    int rank, size;
    char configFile[NAME_LENGTH] = "default.conf", hostname[NAME_LENGTH],
         **files;
    int fileNr, status;
    double wtime;
    Params cfg;
    MpiParams mpi_cfg;
    MPI_Datatype cfg_type;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    setupTypes(&cfg_type);
    MPI_Type_commit(&cfg_type);
    initCL(&cfg);
    if (rank == 0) {
        parseCL(&cfg, &argc, &argv);
        if (argc > 1)
            strncpy(configFile, argv[1], NAME_LENGTH - 1);
        parseFileCL(&cfg, configFile);
        if (argc > 2) {
            dumpCL(stdout, "# ", &cfg);
            return EXIT_SUCCESS;
        }
        cfg2MpiCfg(&cfg, &mpi_cfg);
    }
    MPI_Bcast(&mpi_cfg, 1, cfg_type, 0, MPI_COMM_WORLD);
    mpiCfg2Cfg(&mpi_cfg, &cfg);
    if (cfg.verbose) {
        char label[10];
        sprintf(label, "# [%03d]: ", rank);
        dumpCL(stderr, label, &cfg);
    }
    if (!(files = (char **) calloc(cfg.nrFiles, sizeof(char *)))) {
        fprintf(stderr, "can't allocate files\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    gethostname(hostname, NAME_LENGTH);
    MPI_Barrier(MPI_COMM_WORLD);
    wtime = MPI_Wtime();
    printf("%s[%03d]\twrite\t%.6lf\n",
           hostname, rank, timeOperation(writeFile, cfg, files, &status));
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("global\twrite\t%.6lf\n", MPI_Wtime() - wtime);
    }
    if (status != 0)
        MPI_Abort(MPI_COMM_WORLD, status);
    fflush(stdout);
    if (cfg.doRead) {
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime();
        printf("%s[%03d]\tread\t%.6lf\n",
               hostname, rank, timeOperation(readFile, cfg, files,
                                             &status));
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("global\twrite\t%.6lf\n", MPI_Wtime() - wtime);
        }
        if (status != 0)
            MPI_Abort(MPI_COMM_WORLD, status);
        fflush(stdout);
    }
    if (cfg.doRandomRead) {
        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime();
        printf("%s[%03d]\trandom read\t%.6lf\n",
               hostname, rank, timeOperation(randomReadFile, cfg, files,
                                       &status));
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("global\twrite\t%.6lf\n", MPI_Wtime() - wtime);
        }
        if (status != 0)
            MPI_Abort(MPI_COMM_WORLD, status);
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0 && cfg.cleanUp)
        cleanUp(cfg, files);
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++)
        free(files[fileNr]);
    free(files);
    printf("%s[%03d] finished succesfully\n", hostname, rank);
    finalizeCL(&cfg);
    MPI_Type_free(&cfg_type);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

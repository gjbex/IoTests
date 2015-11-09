#include <mpi.h>

#include "mpi_utils.h"

/*
	int nrFiles;
	int doRead;
	int doRandomRead;
	int bufferSize;
	int cleanUp;
	int verbose;
	long fileSize;
	long randomReads;
	char path[NAME_LENGTH];
*/

void setupTypes(MPI_Datatype *cfg_type) {
    int i;
    MpiParams mpi_cfg;
    int blocks[] = {6, 2, NAME_LENGTH};
    MPI_Aint displacements[3], base;
    MPI_Datatype types[] = {MPI_INT, MPI_LONG, MPI_CHAR};
    MPI_Get_address(&(mpi_cfg.nrFiles), displacements);
    MPI_Get_address(&(mpi_cfg.fileSize), displacements + 1);
    MPI_Get_address(&(mpi_cfg.path[0]), displacements + 2);
    base = displacements[0];
    for (i = 0; i < 3; i++) {
        displacements[i] -= base;
    }
    MPI_Type_create_struct(3, blocks, displacements, types, cfg_type);
}

void cfg2MpiCfg(const Params *cfg, MpiParams *mpi_cfg) {
    int i;
    for (i = 0; i < NAME_LENGTH; i++)
        mpi_cfg->path[i] = cfg->path[i];
    mpi_cfg->nrFiles = cfg->nrFiles;
    mpi_cfg->doRead = cfg->doRead;
    mpi_cfg->doRandomRead = cfg->doRandomRead;
    mpi_cfg->bufferSize = cfg->bufferSize;
    mpi_cfg->cleanUp = cfg->cleanUp;
    mpi_cfg->verbose = cfg->verbose;
    mpi_cfg->fileSize = cfg->fileSize;
    mpi_cfg->randomReads = cfg->randomReads;
}

void mpiCfg2Cfg(const MpiParams *mpi_cfg, Params *cfg) {
    int i;
    for (i = 0; i < NAME_LENGTH; i++)
        cfg->path[i] = mpi_cfg->path[i];
    cfg->nrFiles = mpi_cfg->nrFiles;
    cfg->doRead = mpi_cfg->doRead;
    cfg->doRandomRead = mpi_cfg->doRandomRead;
    cfg->bufferSize = mpi_cfg->bufferSize;
    cfg->cleanUp = mpi_cfg->cleanUp;
    cfg->verbose = mpi_cfg->verbose;
    cfg->fileSize = mpi_cfg->fileSize;
    cfg->randomReads = mpi_cfg->randomReads;
}

void compute_bounds(int size, int rank, long n,
                    long *lbound, long *ubound) {
    long chunk_size = n/size, rest = n % size;
    *lbound = MIN(rank, size - rest)*chunk_size +
        MAX(0, rank - (size - rest))*(chunk_size + 1);
    *ubound = *lbound + (rank >= size - rest ? chunk_size + 1 : chunk_size);
}

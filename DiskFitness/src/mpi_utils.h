#ifndef MPI_UTILS_HDR
#define MPI_UTILS_HDR

#include "defs.h"
#include "config.h"

typedef struct {
	int nrFiles;
	int doRead;
	int doRandomRead;
	int bufferSize;
	int cleanUp;
	int verbose;
	long fileSize;
	long randomReads;
	char path[NAME_LENGTH];
} MpiParams;

void setupTypes(MPI_Datatype *cfg_type);
void cfg2MpiCfg(const Params *cfg, MpiParams *mpi_cfg);
void mpiCfg2Cfg(const MpiParams *mpi_cfg, Params *cfg);

#endif

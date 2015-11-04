#ifndef CONFIG_HDR
#define CONFIG_HDR

#include <stdio.h>

typedef struct {
	int nrFiles;
	char * path;
	long fileSize;
	int bufferSize;
	int doRead;
	int doRandomRead;
	long randomReads;
	int cleanUp;
	int verbose;
} Params;

void initCL(Params *params);
void parseCL(Params *params, int *argc, char **argv[]);
void parseFileCL(Params *params, char *fileName);
void dumpCL(FILE *fp, char prefix[], Params *params);
void finalizeCL(Params *params);
void printHelpCL(FILE *fp);

#endif
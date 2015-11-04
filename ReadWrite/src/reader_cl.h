#ifndef READER_CL_HDR
#define READER_CL_HDR

#include <stdio.h>

typedef struct {
	char * file;
	char * mode;
	int buffer;
	int verbose;
} Params;

void initCL(Params *params);
void parseCL(Params *params, int *argc, char **argv[]);
void parseFileCL(Params *params, char *fileName);
void dumpCL(FILE *fp, char prefix[], Params *params);
void finalizeCL(Params *params);
void printHelpCL(FILE *fp);

#endif
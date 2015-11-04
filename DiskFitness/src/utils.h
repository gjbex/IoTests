#ifndef UTILS_HDR
#define UTILS_HDR

#include "config.h"

char *createBuffer(Params cfg, int *status);
char *initBuffer(Params cfg, int *status);
void cleanUp(Params cfg, char **files);
double timeOperation(int (*op)(Params, char **), Params cfg,
                     char **files, int *status);

#endif

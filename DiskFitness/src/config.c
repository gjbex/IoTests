#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "config_aux.h"

#define MAX_CL_OPT_LEN 128
#define MAX_CL_LINE_LEN 1024

void initCL(Params *params) {
	params->nrFiles = 1;
	int len;
	len = strlen("/node_scratch/tmp-XXXXXX");
	if (!(params->path = (char *) calloc(len + 1, sizeof(char))))
		errx(EXIT_CL_ALLOC_FAIL, "can not allocate path field");
	strncpy(params->path, "/node_scratch/tmp-XXXXXX", len + 1);
	params->fileSize = 10000000;
	params->bufferSize = 65536;
	params->doRead = 1;
	params->doRandomRead = 1;
	params->randomReads = 1000000;
	params->cleanUp = 1;
	params->verbose = 0;
}

void parseCL(Params *params, int *argc, char **argv[]) {
	char *argv_str;
	int i = 1;
	while (i < *argc) {
		 if (!strncmp((*argv)[i], "-?", 3)) {
			printHelpCL(stderr);
			finalizeCL(params);
			exit(EXIT_SUCCESS);
		}
		if (!strncmp((*argv)[i], "-nrFiles", 9)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-nrFiles' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->nrFiles = atoi(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-path", 6)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-path' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->path);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->path = strncpy(tmp, argv_str, len + 1);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-fileSize", 10)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-fileSize' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->fileSize = atol(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-bufferSize", 12)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-bufferSize' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->bufferSize = atoi(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-doRead", 8)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-doRead' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->doRead = atoi(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-doRandomRead", 14)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-doRandomRead' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->doRandomRead = atoi(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-randomReads", 13)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-randomReads' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->randomReads = atol(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-cleanUp", 9)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-cleanUp' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->cleanUp = atoi(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-verbose", 9)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-verbose' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->verbose = atoi(argv_str);
			i++;
			continue;
		}
		break;
	}
	if (i > 1) {
		(*argv)[i - 1] = (*argv)[0];
		*argv = &((*argv)[i - 1]);
		*argc -= (i - 1);
	}
}

void parseFileCL(Params *params, char *fileName) {
	char line_str[MAX_CL_LINE_LEN];
	char argv_str[MAX_CL_LINE_LEN];
	FILE *fp;
	if (!(fp = fopen(fileName, "r"))) {
		fprintf(stderr, "### error: can not open file '%s'\n", fileName);
		exit(EXIT_CL_FILE_OPEN_FAIL);
	}
	while (fgets(line_str, MAX_CL_LINE_LEN, fp)) {
		if (isCommentCL(line_str)) continue;
		if (isEmptyLineCL(line_str)) continue;
		if (sscanf(line_str, "nrFiles = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-nrFiles' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->nrFiles = atoi(argv_str);
			continue;
		}
		if (sscanf(line_str, "path = %[^\n]", argv_str) == 1) {
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-path' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->path);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->path = strncpy(tmp, argv_str, len + 1);
			stripQuotesCL(params->path);
			continue;
		}
		if (sscanf(line_str, "fileSize = %[^\n]", argv_str) == 1) {
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-fileSize' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->fileSize = atol(argv_str);
			continue;
		}
		if (sscanf(line_str, "bufferSize = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-bufferSize' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->bufferSize = atoi(argv_str);
			continue;
		}
		if (sscanf(line_str, "doRead = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-doRead' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->doRead = atoi(argv_str);
			continue;
		}
		if (sscanf(line_str, "doRandomRead = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-doRandomRead' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->doRandomRead = atoi(argv_str);
			continue;
		}
		if (sscanf(line_str, "randomReads = %[^\n]", argv_str) == 1) {
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-randomReads' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->randomReads = atol(argv_str);
			continue;
		}
		if (sscanf(line_str, "cleanUp = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-cleanUp' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->cleanUp = atoi(argv_str);
			continue;
		}
		if (sscanf(line_str, "verbose = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-verbose' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->verbose = atoi(argv_str);
			continue;
		}
		fprintf(stderr, "### warning, line can not be parsed: '%s'\n", line_str);
	}
	fclose(fp);
}

void dumpCL(FILE *fp, char prefix[], Params *params) {
	fprintf(fp, "%snrFiles = %d\n", prefix, params->nrFiles);
	fprintf(fp, "%spath = '%s'\n", prefix, params->path);
	fprintf(fp, "%sfileSize = %ld\n", prefix, params->fileSize);
	fprintf(fp, "%sbufferSize = %d\n", prefix, params->bufferSize);
	fprintf(fp, "%sdoRead = %d\n", prefix, params->doRead);
	fprintf(fp, "%sdoRandomRead = %d\n", prefix, params->doRandomRead);
	fprintf(fp, "%srandomReads = %ld\n", prefix, params->randomReads);
	fprintf(fp, "%scleanUp = %d\n", prefix, params->cleanUp);
	fprintf(fp, "%sverbose = %d\n", prefix, params->verbose);
}

void finalizeCL(Params *params) {
	free(params->path);
}

void printHelpCL(FILE *fp) {
	fprintf(fp, "  -nrFiles <integer>\n  -path <string>\n  -fileSize <long integer>\n  -bufferSize <integer>\n  -doRead <integer>\n  -doRandomRead <integer>\n  -randomReads <long integer>\n  -cleanUp <integer>\n  -verbose <integer>\n  -?: print this message");
}
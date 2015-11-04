#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "writer_cl.h"
#include "writer_cl_aux.h"

#define MAX_CL_OPT_LEN 128
#define MAX_CL_LINE_LEN 1024

void initCL(Params *params) {
	int len;
	len = strlen("output.dat");
	if (!(params->file = (char *) calloc(len + 1, sizeof(char))))
		errx(EXIT_CL_ALLOC_FAIL, "can not allocate file field");
	strncpy(params->file, "output.dat", len + 1);
	len = strlen("text");
	if (!(params->mode = (char *) calloc(len + 1, sizeof(char))))
		errx(EXIT_CL_ALLOC_FAIL, "can not allocate mode field");
	strncpy(params->mode, "text", len + 1);
	params->size = 1024;
	params->buffer = 128;
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
		if (!strncmp((*argv)[i], "-file", 6)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-file' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->file);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->file = strncpy(tmp, argv_str, len + 1);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-mode", 6)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-mode' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->mode);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->mode = strncpy(tmp, argv_str, len + 1);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-size", 6)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-size' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->size = atol(argv_str);
			i++;
			continue;
		}
		if (!strncmp((*argv)[i], "-buffer", 8)) {
			shiftCL(&i, *argc, *argv);
			argv_str = (*argv)[i];
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-buffer' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->buffer = atoi(argv_str);
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
		if (sscanf(line_str, "file = %[^\n]", argv_str) == 1) {
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-file' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->file);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->file = strncpy(tmp, argv_str, len + 1);
			stripQuotesCL(params->file);
			continue;
		}
		if (sscanf(line_str, "mode = %[^\n]", argv_str) == 1) {
			if (!1) {
				fprintf(stderr, "### error: invalid value for option '-mode' of type char *\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			char *tmp;
			int len = strlen(argv_str);
			free(params->mode);
			if (!(tmp = (char *) calloc(len + 1, sizeof(char))))
				errx(EXIT_CL_ALLOC_FAIL, "can not allocate char* field");
			params->mode = strncpy(tmp, argv_str, len + 1);
			stripQuotesCL(params->mode);
			continue;
		}
		if (sscanf(line_str, "size = %[^\n]", argv_str) == 1) {
			if (!isLongCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-size' of type long\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->size = atol(argv_str);
			continue;
		}
		if (sscanf(line_str, "buffer = %[^\n]", argv_str) == 1) {
			if (!isIntCL(argv_str, 0)) {
				fprintf(stderr, "### error: invalid value for option '-buffer' of type int\n");
				exit(EXIT_CL_INVALID_VALUE);
			}
			params->buffer = atoi(argv_str);
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
	fprintf(fp, "%sfile = '%s'\n", prefix, params->file);
	fprintf(fp, "%smode = '%s'\n", prefix, params->mode);
	fprintf(fp, "%ssize = %ld\n", prefix, params->size);
	fprintf(fp, "%sbuffer = %d\n", prefix, params->buffer);
	fprintf(fp, "%sverbose = %d\n", prefix, params->verbose);
}

void finalizeCL(Params *params) {
	free(params->file);
	free(params->mode);
}

void printHelpCL(FILE *fp) {
	fprintf(fp, "  -file <string>\n  -mode <string>\n  -size <long integer>\n  -buffer <integer>\n  -verbose <integer>\n  -?: print this message");
}
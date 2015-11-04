#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "config.h"
#include "defs.h"
#include "errors.h"
#include "tests.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    char configFile[NAME_LENGTH] = "default.conf", hostname[NAME_LENGTH],
         **files;
    int fileNr, status;
    Params cfg;
    initCL(&cfg);
    parseCL(&cfg, &argc, &argv);
    if (argc > 1)
        strncpy(configFile, argv[1], NAME_LENGTH - 1);
    parseFileCL(&cfg, configFile);
    if (argc > 2) {
        dumpCL(stdout, "# ", &cfg);
        return EXIT_SUCCESS;
    }
    if (!(files = (char **) calloc(cfg.nrFiles, sizeof(char *))))
        errx(EXIT_FAILURE, "can't allocate files");
    gethostname(hostname, NAME_LENGTH);
    printf("%s\twrite\t%.6lf\n",
           hostname, timeOperation(writeFile, cfg, files, &status));
    if (status != 0)
        return status;
    fflush(stdout);
    if (cfg.doRead) {
        printf("%s\tread\t%.6lf\n",
               hostname, timeOperation(readFile, cfg, files, &status));
        if (status != 0)
            return status;
        fflush(stdout);
    }
    if (cfg.doRandomRead) {
        printf("%s\trandom read\t%.6lf\n",
               hostname, timeOperation(randomReadFile, cfg, files,
                                       &status));
        if (status != 0)
            return status;
        fflush(stdout);
    }
    if (cfg.cleanUp)
        cleanUp(cfg, files);
    for (fileNr = 0; fileNr < cfg.nrFiles; fileNr++)
        free(files[fileNr]);
    free(files);
    printf("%s finished succesfully\n", hostname);
    finalizeCL(&cfg);
    return EXIT_SUCCESS;
}

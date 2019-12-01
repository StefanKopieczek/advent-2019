#include "adventfiles.h"

void advProcessFile(char *path, adv_line_handler handleLine, void *context) {
    FILE *f = fopen(path, "r");
    char line[10000];
    while (1) {
        char *s = fgets(line, 10000, f);
        if (s == 0) {
            break;
        }

        char *trimmed = strtok(line, "\n");
        if (trimmed[0] == '\n') {
            handleLine("", context);
        } else {
            handleLine(trimmed, context);
        }
    }

    fclose(f);
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*adv_line_handler) (char *line, void *context);

void adv_forLineInFile(char *path, adv_line_handler handleLine, void *context);

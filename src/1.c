#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "adventfiles.h"

#define INPUT "./inputs/1.txt"

int calculateFuel(char *, bool);
void handleLine(char *line, void *context);
int weightToFuel(int);
int weightToFuelRecursive(int);

typedef struct {
    int totalWeight;
    bool isRecursive;
} state;

int main(int argc, char **argv) {
    int fuelNeededNaive = calculateFuel(INPUT, false);
    int fuelNeededRecursive = calculateFuel(INPUT, true);
    printf("Fuel required (naive): %d.\n", fuelNeededNaive);
    printf("Fuel required (recursive): %d.\n", fuelNeededRecursive);
    return 0;
}

int calculateFuel(char *filename, bool recursive) {
    state s = {0, recursive};
    advProcessFile(filename, &handleLine, &s);
    return s.totalWeight;
}

void handleLine(char *line, void *context) {
    state *s = (state *)context;
    if (s->isRecursive) {
        s->totalWeight += weightToFuelRecursive(atoi(line));
    } else {
        s->totalWeight += weightToFuel(atoi(line));
    }
}

int weightToFuel(int moduleWeight) {
    return (moduleWeight / 3) - 2;
}

int weightToFuelRecursive(int moduleWeight) {
    int total = 0;
    int newWeight = weightToFuel(moduleWeight);
    while (newWeight > 0) {
        total += newWeight;
        newWeight = weightToFuel(newWeight);
    }

    return total;
}

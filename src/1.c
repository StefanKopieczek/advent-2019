#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INPUT "./inputs/1.txt"

int calculateFuel(char *, bool);
int weightToFuel(int);
int weightToFuelRecursive(int);

int main(int argc, char **argv) {
    int fuelNeededNaive = calculateFuel(INPUT, false);
    int fuelNeededRecursive = calculateFuel(INPUT, true);
    printf("Fuel required (naive): %d.\n", fuelNeededNaive);
    printf("Fuel required (recursive): %d.\n", fuelNeededRecursive);
    return 0;
}

int calculateFuel(char *filename, bool recursive) {
    FILE *input = fopen(filename, "r");
    char line[100];
    int total = 0;
    while (1) {
        char *s = fgets(line, 100, input);
        if (s == 0) {
            break;
        }

        int moduleWeight = atoi(strtok(line, "\n"));
        if (recursive) {
            total += weightToFuelRecursive(moduleWeight);
        } else {
            total += weightToFuel(moduleWeight);
        }
    }

    fclose(input);
    return total;
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

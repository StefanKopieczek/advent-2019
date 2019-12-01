#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "./inputs/1.txt"

int calculateFuel(char *);
int weightToFuel(int);

int main(int argc, char **argv) {
    int fuelNeeded = calculateFuel(INPUT);
    printf("Fuel required: %d.\n", fuelNeeded);
    return 0;
}

int calculateFuel(char *filename) {
    FILE *input = fopen(filename, "r");
    char line[100];
    int total = 0;
    while (1) {
        char *s = fgets(line, 100, input);
        if (s == 0) {
            break;
        }

        int moduleWeight = atoi(strtok(line, "\n"));
        total += weightToFuel(moduleWeight);
    }

    fclose(input);
    return total;
}

int weightToFuel(int moduleWeight) {
    return (moduleWeight / 3) - 2;
}

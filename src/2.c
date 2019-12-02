#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_PATH "./inputs/2.txt"

typedef struct {
    int *instructions;
    int size;
} *program, s_program;

program readProgram(char*, int);
void runProgram(program, int noun, int verb);
void freeProgram(program);
bool loadArgs(program, int, int *, int *, int *);
void seekInputsForOutput(char *, int, int *, int*);

int main(int argc, char **argv) {
    program prog = readProgram(PROGRAM_PATH, 100000);

    runProgram(prog, 12, 2);
    printf("Instruction 0 of program with input 1202 is %d.\n", prog->instructions[0]);

    int noun;
    int verb;
    seekInputsForOutput(PROGRAM_PATH, 19690720, &noun, &verb);
    printf("Output produced with noun %d and verb %d.\n", noun, verb);

    freeProgram(prog);
}

program readProgram(char *path, int size) {
    program prog = malloc(sizeof(s_program));
    prog->instructions = malloc(sizeof(int) * size);
    prog->size = size;

    FILE *fPtr = fopen(path, "r");
    int index = 0;
    int item;

    while(index < size && fscanf(fPtr, "%d,", &item) != -1) {
        prog ->instructions[index] = item;
        index++;
    }

    if (index < size) {
        // Last element needs special handling as it doesn't terminate in a ','
        fscanf(fPtr, "%d", &item);
        prog->instructions[index] = item;
    }

    if (index >= size) {
        fprintf(stderr, "Out of space at index %d when loading program. Truncating.\n", index);
    }

    fclose(fPtr);
    return prog;
}

void freeProgram(program prog) {
    free(prog->instructions);
    free(prog);
}

void runProgram(program p, int noun, int verb) {
    p->instructions[1] = noun;
    p->instructions[2] = verb;

    int index = 0;
    while (1) {
        if (index >= p->size) {
            fprintf(stderr, "Current instruction pointer %d out of bounds (max %d).\n", index, p->size);
            return;
        }

        int instr = p->instructions[index];
        if (instr == 1 || instr == 2) {
            int a, b, dest;
            bool success = loadArgs(p, index, &a, &b, &dest);
            if (!success) {
                fprintf(stderr, "Halting execution due to error.\n");
                return;
            }
            if (instr == 1) {
                p->instructions[dest] = p->instructions[a] + p->instructions[b];
            } else {
                p->instructions[dest] = p->instructions[a] * p->instructions[b];
            }
            index += 4;
        } else if (instr == 99) {
            return;
        }  else {
            fprintf(stderr, "Illegal instruction %d at index %d.\n", instr, index);
            return;
        }
    }
}

void seekInputsForOutput(char *path, int output, int *noun, int *verb) {
    for (int nounGuess = 0; nounGuess < 100; nounGuess++) {
        for (int verbGuess = 0; verbGuess < 100; verbGuess++) {
            program p = readProgram(path, 10000);
            runProgram(p, nounGuess, verbGuess);
            if (p->instructions[0] == output) {
                *noun = nounGuess;
                *verb = verbGuess;
                return;
            }
        }
    }

    *noun = 1;
    *verb = -1;
}

bool loadArgs(program p, int index, int *a, int *b, int *dest) {
    if (index + 3 >= p->size) {
        fprintf(stderr, "Out of bounds during argument load at index %d (limit %d).\n", index, p->size);
        return false;
    }

    *a = p->instructions[++index];
    *b = p->instructions[++index];
    *dest = p->instructions[++index];

    if (*a >= p->size) {
        fprintf(stderr, "Operand a destination %d out of bounds (limit %d).\n", *a, p->size);
        return false;
    }
    if (*b >= p->size) {
        fprintf(stderr, "Operand b destination %d out of bounds (limit %d).\n", *b, p->size);
        return false;
    }
    if (*dest >= p->size) {
        fprintf(stderr, "Operation destination %d out of bounds (limit %d).\n", *dest, p->size);
        return false;
    }

    return true;
}

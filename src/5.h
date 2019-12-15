#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int *values;
    int count;
    int capacity;
} Tape;

Tape *tape_init();
void tape_free(Tape *);
Tape *tape_parse(FILE *);
void tape_append(Tape *, int);
int tape_get(Tape *, int);
bool tape_update(Tape *tape, int idx, int value);

typedef enum { POSITION, IMMEDIATE } AddressMode;

typedef int Instruction;

typedef enum {
    ADD = 1,
    MULTIPLY = 2,
    INPUT = 3,
    OUTPUT = 4,
    HALT = 99
} Opcode;

Opcode get_opcode(Instruction instruction);
AddressMode get_parameter_address_mode(Instruction instruction, int paramIndex);

typedef enum { RUNNING, COMPLETE, ERROR } Status;

typedef struct {
    Tape *tape;
    int ptr;
    Status status;
} State;

State *state_init(Tape *);
void state_free(State *); // This must *not* free the tape!
bool is_running(State *);
int read_next_value(State *);
void advance_pointer(State *, int);
Instruction read_instruction(State *);
int load_parameter(State *state, Instruction instr, int paramIdx);
int load_destination(State *state, Instruction instr, int paramIdx);
void do_add(State *, Instruction);
void do_multiply(State *, Instruction);
void do_input(State *, Instruction);
void do_output(State *, Instruction);
void do_halt(State *, Instruction);
void update_or_error(State *state, int dest, int value);
void tick(State *);
void run(State *);

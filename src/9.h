#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

long max(long, long);

typedef struct {
    long *values;
    long count;
    long capacity;
} Tape;

Tape *tape_init();
void tape_free(Tape *);
Tape *tape_parse(FILE *);
void tape_ensure_capacity(Tape *, long);
void tape_append(Tape *, long);
long tape_get(Tape *, long);
bool tape_update(Tape *tape, long idx, long value);

typedef enum { POSITION, IMMEDIATE } AddressMode;

typedef long Instruction;

typedef enum {
    ADD = 1,
    MULTIPLY = 2,
    INPUT = 3,
    OUTPUT = 4,
    JUMP_IF_TRUE = 5,
    JUMP_IF_FALSE = 6,
    LESS_THAN = 7,
    EQUALS = 8,
    HALT = 99
} Opcode;

Opcode get_opcode(Instruction instruction);
AddressMode get_parameter_address_mode(Instruction instruction, int paramIndex);

typedef enum { RUNNING, COMPLETE, ERROR } Status;

typedef struct {
    Tape *tape;
    long ptr;
    Status status;
} State;

State *state_init(Tape *);
void state_free(State *); // This must *not* free the tape!
bool is_running(State *);
long read_next_value(State *);
void advance_pointer(State *, long);
Instruction read_instruction(State *);
long load_parameter(State *state, Instruction instr, int paramIdx);
long load_destination(State *state, Instruction instr, int paramIdx);
void do_add(State *, Instruction);
void do_multiply(State *, Instruction);
void do_input(State *, Instruction);
void do_output(State *, Instruction);
void do_jump_if_true(State *, Instruction);
void do_jump_if_false(State *, Instruction);
void do_less_than(State *, Instruction);
void do_equals(State *, Instruction);
void do_halt(State *, Instruction);
void update_or_error(State *state, long dest, long value);
void tick(State *);
void run(State *);

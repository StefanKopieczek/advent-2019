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

struct s_QueueCell {
    int value;
    struct s_QueueCell *next;
};

typedef struct {
    struct s_QueueCell *start;
} Queue;

Queue *queue_init();
void queue_free(Queue *);
int queue_retrieve(Queue *);
void queue_append(Queue *, int);
bool queue_is_empty(Queue *);

typedef enum { POSITION, IMMEDIATE } AddressMode;

typedef int Instruction;

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
    int ptr;
    Status status;
    Queue *input;
    Queue *output;
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
void do_jump_if_true(State *, Instruction);
void do_jump_if_false(State *, Instruction);
void do_less_than(State *, Instruction);
void do_equals(State *, Instruction);
void do_halt(State *, Instruction);
void update_or_error(State *state, int dest, int value);
void tick(State *);
void run_until_output(State *);

int get_thruster_signal(int* phaseSettings);
int get_thruster_signal_feedback(int* phaseSettings);
bool phase_settings_are_valid(int* phaseSettings);
bool increment_phase_settings(int* phaseSettings);

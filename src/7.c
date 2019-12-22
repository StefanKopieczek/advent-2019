#include "7.h"

#define TAPE_PATH "./inputs/7.txt"
#define INITIAL_TAPE_LENGTH 50
#define NUM_AMPLIFIERS 5

Tape *load_from_path(char *path) {
    FILE *f = fopen(path, "r");
    Tape *tape = tape_parse(f);
    fclose(f);
    return tape;
}

int main(int argc, char **argv) {
    int phaseSettings[NUM_AMPLIFIERS];
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        phaseSettings[i] = 0;
    }

    int bestOutput = -1;
    while (increment_phase_settings(phaseSettings)) {
        if (phase_settings_are_valid(phaseSettings)) {
            int thrusterSignal = get_thruster_signal(phaseSettings);
            if (thrusterSignal > bestOutput) {
                bestOutput = thrusterSignal;
            }
        }
    }

    printf("The best possible thruster output is %d.\n", bestOutput);
}

bool increment_phase_settings(int *settings) {
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        settings[i] = (settings[i] + 1) % NUM_AMPLIFIERS;
        if (settings[i] != 0) {
            return true;
        }
    }
    return settings[NUM_AMPLIFIERS-1] != 0;
}

bool phase_settings_are_valid(int *settings) {
    int indices[NUM_AMPLIFIERS];
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        indices[i] = 0;
    }
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        indices[settings[i]]++;
    }
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        if (indices[i] == 0) {
            return false;
        }
    }
    return true;
}

int get_thruster_signal(int *phaseSettings) {
    Tape *tapes[NUM_AMPLIFIERS];
    State *states[NUM_AMPLIFIERS];
    int last_output = 0;
    for (int i = 0; i < NUM_AMPLIFIERS; i++) {
        tapes[i] = load_from_path(TAPE_PATH);
        states[i] = state_init(tapes[i]);
        queue_append(states[i]->input, phaseSettings[i]);
        queue_append(states[i]->input, last_output);
        last_output = run_until_output(states[i]);
        state_free(states[i]);
        tape_free(tapes[i]);
    }

    return last_output;
}

int run_until_output(State *state) {
    while(is_running(state) && queue_is_empty(state->output)) {
        tick(state);
    }

    if (state->status == ERROR) {
        fprintf(stderr, "ERROR: program %p exited with status ERROR.\n", state);
        return -1;
	} else if (queue_is_empty(state->output)) {
        fprintf(stderr, "ERROR: program %p exited with no output.\n", state);
        return -1;
    } else {
        return (queue_retrieve(state->output));
    }
}

void tick(State *state) {
    Instruction instr = read_instruction(state);
    Opcode opcode = get_opcode(instr);
    switch (opcode) {
        case ADD:
            do_add(state, instr);
            break;
        case MULTIPLY:
            do_multiply(state, instr);
            break;
        case INPUT:
            do_input(state, instr);
            break;
        case OUTPUT:
            do_output(state, instr);
            break;
        case JUMP_IF_TRUE:
            do_jump_if_true(state, instr);
            break;
        case JUMP_IF_FALSE:
            do_jump_if_false(state, instr);
            break;
        case LESS_THAN:
            do_less_than(state, instr);
            break;
        case EQUALS:
            do_equals(state, instr);
            break;
        case HALT:
            do_halt(state, instr);
            break;
        default:
            fprintf(stderr, "ERROR: Unknown opcode %d encountered in instruction %d at address %d.\n", opcode, instr, state->ptr);
            state->status = ERROR;
    }
}

Instruction read_instruction(State *state) {
    return read_next_value(state);
}

Opcode get_opcode(Instruction instr) {
    // The opcode is made up of the last two digits of the instruction.
    return instr % 100;
}

void do_add(State *state, Instruction instr) {
    int a = load_parameter(state, instr, 1);
    int b = load_parameter(state, instr, 2);
    int dst = load_destination(state, instr, 3);
    update_or_error(state, dst, a + b);
}

void do_multiply(State *state, Instruction instr) {
    int a = load_parameter(state, instr, 1);
    int b = load_parameter(state, instr, 2);
    int dst = load_destination(state, instr, 3);
    update_or_error(state, dst, a * b);
}

void do_input(State *state, Instruction instr) {
    int dst = load_destination(state, instr, 1);
    if (queue_is_empty(state->input)) {
        fprintf(stderr, "Input requested, but input queue is empty!\n");
        state->status = ERROR;
        return;
    }

    int value = queue_retrieve(state->input);
    update_or_error(state, dst, value);
}

void do_output(State *state, Instruction instr) {
    int value = load_parameter(state, instr, 1);
    queue_append(state->output, value);
}

void do_jump_if_true(State *state, Instruction instr) {
    int conditional = load_parameter(state, instr, 1);
    int new_ptr = load_parameter(state, instr, 2);
    if (conditional != 0) {
        state->ptr = new_ptr;
    }
}

void do_jump_if_false(State *state, Instruction instr) {
    int conditional = load_parameter(state, instr, 1);
    int new_ptr = load_parameter(state, instr, 2);
    if (conditional == 0) {
        state->ptr = new_ptr;
    }
}

void do_less_than(State *state, Instruction instr) {
    int a = load_parameter(state, instr, 1);
    int b = load_parameter(state, instr, 2);
    int dst = load_destination(state, instr, 3);
    int result = (a < b) ? 1 : 0;
    update_or_error(state, dst, result);
}

void do_equals(State *state, Instruction instr) {
    int a = load_parameter(state, instr, 1);
    int b = load_parameter(state, instr, 2);
    int dst = load_destination(state, instr, 3);
    int result = (a == b) ? 1 : 0;
    update_or_error(state, dst, result);
}

void do_halt(State *state, Instruction instr) {
    state->status = COMPLETE;
}

int load_parameter(State *state, Instruction instr, int paramIdx) {
    int rawParam = read_next_value(state);
    AddressMode addressMode = get_parameter_address_mode(instr, paramIdx);
    switch (addressMode) {
        case POSITION:
            // In POSITION mode, the tape value is a pointer to the location holding the parameter.
            return tape_get(state->tape, rawParam);
        case IMMEDIATE:
            // In IMMEDIATE mode, the tape value is a literal.
            return rawParam;
        default:
            fprintf(stderr, "ERROR: Unknown address mode %d.\n", addressMode);
            state->status = ERROR;
            return 0;
    }
}

int load_destination(State *state, Instruction instr, int paramIdx) {
    int destination = read_next_value(state);
    AddressMode addressMode = get_parameter_address_mode(instr, paramIdx);
    if (addressMode != POSITION) {
        fprintf(stderr, "ERROR: A destination parameter had invalid address mode %d."
                        " Destinations must have POSITION mode.\n", addressMode);
        state->status = ERROR;
    }
    return destination;
}

AddressMode get_parameter_address_mode(Instruction instr, int paramIdx) {
    // The address mode for parameter 'i' is the digit with offset (i+2) from the right of the instruction.
    instr /= 100;
    for (int i = 1; i < paramIdx; i++) {
        instr /= 10;
    }
    return instr % 10;
}

int read_next_value(State *state) {
    int value = tape_get(state->tape, state->ptr);
    advance_pointer(state, 1);
    return value;
}

void update_or_error(State *state, int dest, int value) {
    if (!tape_update(state->tape, dest, value)) {
        fprintf(stderr, "ERROR: failed to update the tape.\n");
        state->status = ERROR;
    }
}

void advance_pointer(State *state, int offset) {
    state->ptr += offset;
}

State *state_init(Tape *tape) {
    State *state = malloc(sizeof(State));
    state->tape = tape;
    state->ptr = 0;
    state->status = RUNNING;
    state->input = queue_init();
    state->output = queue_init();
    return state;
}

void state_free(State *state) {
    queue_free(state->input);
    queue_free(state->output);
    free(state);
}

bool is_running(State *state) {
    return state->status == RUNNING;
}

Tape *tape_parse(FILE *f) {
    Tape *tape = tape_init();
    int value;
    while (1) {
        int read = fscanf(f, "%d,", &value);
        if (read <= 0) {
            break;
        }
        tape_append(tape, value);
    }
    return tape;
}

Tape *tape_init() {
    Tape *tape = malloc(sizeof(Tape));
    tape->capacity = INITIAL_TAPE_LENGTH;
    tape->values = malloc(sizeof(int) * tape->capacity);
    tape->count = 0;
    return tape;
}

void tape_free(Tape *tape) {
    free(tape->values);
    free(tape);
}

void tape_append(Tape *tape, int value) {
    if (tape->count * 2 > tape->capacity) {
        tape->capacity *= 2;
        tape->values = realloc(tape->values, sizeof(int) * tape->capacity);
    }

    tape->values[tape->count++] = value;
}

int tape_get(Tape *tape, int idx) {
    if (idx < 0) {
        fprintf(stderr, "ERROR: Attempt to read negative index %d from tape.\n", idx);
        return 0;
    } else if (idx >= tape->count) {
        fprintf(stderr, "ERROR: Attempt to read after end of tape (index %d, tape end %d).\n", idx, tape->count);
        return 0;
    } else {
        return tape->values[idx];
    }
}

bool tape_update(Tape *tape, int idx, int value) {
    if (idx >= tape->count) {
        fprintf(stderr, "ERROR: Attempt to update value after end of tape (index %d, tape end %d).\n", idx, tape->count);
        return false;
    }

    tape->values[idx] = value;
    return true;
}

Queue *queue_init() {
    Queue *queue = malloc(sizeof(Queue));
    queue->start = 0;
    return queue;
}

void queue_free(Queue *queue) {
    struct s_QueueCell *cell = queue->start;
    while (cell != 0) {
        struct s_QueueCell *next = cell->next;
        free(cell);
        cell = next;
    }
    free(queue);
}

int queue_retrieve(Queue *queue) {
    struct s_QueueCell *head = queue->start;
    queue->start = head->next;
    int result = head->value;
    free(head);
    return result;
}

void queue_append(Queue *queue, int value) {
    struct s_QueueCell **ptrToNext = &queue->start;
    while (*ptrToNext != 0) {
        ptrToNext = &(*ptrToNext)->next;
    }

    *ptrToNext = malloc(sizeof(struct s_QueueCell));
    (*ptrToNext)->value = value;
    (*ptrToNext)->next = 0;
}

bool queue_is_empty(Queue *queue) {
    return queue->start == 0;
}

#include "5.h"

#define TAPE_PATH "./inputs/5.txt"
#define INITIAL_TAPE_LENGTH 50

Tape *load_from_path(char *path) {
    FILE *f = fopen(path, "r");
    Tape *tape = tape_parse(f);
    fclose(f);
    return tape;
}

int main(int argc, char **argv) {
    Tape *tape = load_from_path(TAPE_PATH);
    State *program = state_init(tape);
    run(program);
    state_free(program);
    tape_free(tape);
}

void run(State *state) {
    printf("Starting program %p on tape %p.\n", state, state->tape);

    while (is_running(state)) {
        tick(state);
    }

	switch (state->status) {
		case COMPLETE:
			printf("Program %p exited with status COMPLETE.\n", state);
			break;
		case ERROR:
			printf("Program %p exited with status ERROR.\n", state);
			break;
		default:
			printf("Program %p exited with unknown status code %d.\n", state, state->status);
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
    int value;
    printf("Program requests input: ");
    while (1) {
        int read = scanf("%d", &value);
        if (read == 0) {
            printf("No value received. Try again: ");
        } else {
            break;
        }
    }
    update_or_error(state, dst, value);
}

void do_output(State *state, Instruction instr) {
    int value = load_parameter(state, instr, 1);
    printf("Program outputted a value: %d\n", value);
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
    State *state = malloc(sizeof(state));
    state->tape = tape;
    state->ptr = 0;
    state->status = RUNNING;
    return state;
}

void state_free(State *state) {
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

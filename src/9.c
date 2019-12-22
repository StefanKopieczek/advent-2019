#include "9.h"

//#define DEBUG_ENABLE
#include "debug.h"

#define TAPE_PATH "./inputs/9.txt"
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
    debug("Starting tick; pointer location %lld.\n", state->ptr);
    Instruction instr = read_instruction(state);
    debug("\tRead instruction %lld.\n", instr);

    Opcode opcode = get_opcode(instr);
    debug("\tParsed opcode %d.\n", opcode);
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
            fprintf(stderr, "ERROR: Unknown opcode %d encountered in instruction %lld at address %lld.\n", opcode, instr, state->ptr);
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
    debug("\tPerforming addition.\n");
    long long a = load_parameter(state, instr, 1);
    long long b = load_parameter(state, instr, 2);
    long long dst = load_destination(state, instr, 3);
    debug("\t\t%lld+%lld->%lld\n", a, b, dst);
    update_or_error(state, dst, a + b);
}

void do_multiply(State *state, Instruction instr) {
    debug("\tPerforming multiplication.\n");
    long long a = load_parameter(state, instr, 1);
    long long b = load_parameter(state, instr, 2);
    long long dst = load_destination(state, instr, 3);
    debug("\t\t%lld*%lld->%lld\n", a, b, dst);
    update_or_error(state, dst, a * b);
}

void do_input(State *state, Instruction instr) {
    debug("\tRequesting input.\n");
    long long dst = load_destination(state, instr, 1);
    long long value;
    printf("Program requests input: ");
    while (1) {
        int read = scanf("%lld", &value);
        if (read == 0) {
            printf("No value received. Try again: ");
        } else {
            break;
        }
    }
    debug("\t\t(input %lld)->%lld\n", value, dst);
    update_or_error(state, dst, value);
}

void do_output(State *state, Instruction instr) {
    debug("\tOutputting value.\n");
    long long value = load_parameter(state, instr, 1);
    printf("Program outputted a value: %lld\n", value);
}

void do_jump_if_true(State *state, Instruction instr) {
    debug("\tDoing JIT.\n");
    long long conditional = load_parameter(state, instr, 1);
    long long new_ptr = load_parameter(state, instr, 2);
    debug("\t\tif %lld => %lld\n", conditional, new_ptr);
    if (conditional != 0) {
        state->ptr = new_ptr;
        debug("\t\tJumping.\n");
    }
}

void do_jump_if_false(State *state, Instruction instr) {
    debug("\tDoing JIF.\n");
    long long conditional = load_parameter(state, instr, 1);
    long long new_ptr = load_parameter(state, instr, 2);
    debug("\t\tif !%lld => %lld\n", conditional, new_ptr);
    if (conditional == 0) {
        state->ptr = new_ptr;
        debug("\t\tJumping.\n");
    }
}

void do_less_than(State *state, Instruction instr) {
    debug("\tDoing LT.\n");
    long long a = load_parameter(state, instr, 1);
    long long b = load_parameter(state, instr, 2);
    long long dst = load_destination(state, instr, 3);
    long long result = (a < b) ? 1 : 0;
    debug("\t\t(%lld < %lld) [%lld] -> %lld\n", a, b, result, dst);
    update_or_error(state, dst, result);
}

void do_equals(State *state, Instruction instr) {
    debug("\tDoing EQ.\n");
    long long a = load_parameter(state, instr, 1);
    long long b = load_parameter(state, instr, 2);
    long long dst = load_destination(state, instr, 3);
    long long result = (a == b) ? 1 : 0;
    debug("\t\t(%lld == %lld) [%lld] -> %lld\n", a, b, result, dst);
    update_or_error(state, dst, result);
}

void do_halt(State *state, Instruction instr) {
    debug("\tDoing HALT.\n");
    state->status = COMPLETE;
}

long long load_parameter(State *state, Instruction instr, int paramIdx) {
    long long rawParam = read_next_value(state);
    AddressMode addressMode = get_parameter_address_mode(instr, paramIdx);
    switch (addressMode) {
        case POSITION:
            // In POSITION mode, the tape value is a pointer to the location hollding the parameter.
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

long long load_destination(State *state, Instruction instr, int paramIdx) {
    long long destination = read_next_value(state);
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

long long read_next_value(State *state) {
    long long value = tape_get(state->tape, state->ptr);
    advance_pointer(state, 1);
    return value;
}

void update_or_error(State *state, long long dest, long long value) {
    if (!tape_update(state->tape, dest, value)) {
        fprintf(stderr, "ERROR: failed to update the tape.\n");
        state->status = ERROR;
    }
}

void advance_pointer(State *state, long long offset) {
    state->ptr += offset;
}

State *state_init(Tape *tape) {
    State *state = malloc(sizeof(State));
    state->tape = tape;
    state->ptr = 0;
    state->relativeBase = 0;
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
    long long value;
    while (1) {
        int read = fscanf(f, "%lld,", &value);
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
    tape->values = calloc(tape->capacity, sizeof(long long));
    tape->count = 0;
    return tape;
}

void tape_free(Tape *tape) {
    free(tape->values);
    free(tape);
}

void tape_ensure_capacity(Tape *tape, long long newCapacity) {
    while (newCapacity * 2 > tape->capacity) {
        long long olldCapacity = tape->capacity;
        tape->capacity *= 2;
        tape->values = realloc(tape->values, sizeof(long long) * tape->capacity);
        for (long long i=olldCapacity; i < tape->capacity; i++) {
            tape->values[i] = 0;
        }
    }
}

void tape_append(Tape *tape, long long value) {
    tape_ensure_capacity(tape, tape->count);
    tape->values[tape->count++] = value;
}

long long tape_get(Tape *tape, long long idx) {
    if (idx < 0) {
        fprintf(stderr, "ERROR: Attempt to read negative index %lld from tape.\n", idx);
        return 0;
    }

    tape_ensure_capacity(tape, idx);
    tape->count = max(tape->count, idx);
    return tape->values[idx];
}

bool tape_update(Tape *tape, long long idx, long long value) {
    tape_ensure_capacity(tape, idx);
    tape->values[idx] = value;
    tape->count = max(tape->count, idx);
    return true;
}

long long max(long long a, long long b) {
    return (a <= b) ? b : a;
}

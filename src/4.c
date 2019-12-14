#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void get_digits(int guess, int *digits) {
    for (int idx = 5; idx >= 0; idx--) {
        digits[idx] = guess % 10;
        guess /= 10;
    }
}

bool has_two_adjacent(int *digits) {
    int old = -1;
    for (int *digit = digits; digit < &digits[6]; digit++) {
        if (*digit == old) {
            return true;
        }
        old = *digit;
    }

    return false;
}

bool has_exactly_two_adjacent(int *digits) {
    int vals[] = { digits[0], digits[1], digits[2], digits[3], digits[4], digits[5] };
    for (int ptr = 0; ptr < 4; ptr++) {
        if (vals[ptr] == vals[ptr+1]) {
            if (vals[ptr+1] == vals[ptr+2]) {
                // Bork the rest of the group to avoid us matching it later.
                for (int ptr2 = ptr + 1; ptr2 < 6 && vals[ptr2] == vals[ptr]; ptr2++) {
                    vals[ptr2] = -ptr2;
                }
            } else {
                return true;
            }
        }
    }

    return vals[4] == vals[5];
}

bool is_monotonic(int *digits) {
    int old = -1;
    for (int *digit = digits; digit < &digits[6]; digit++) {
        if (*digit < old) {
            return false;
        }
        old = *digit;
    }

    return true;
}

int main(int argv, char **argc) {
    int valid_count_1 = 0;
    int valid_count_2 = 0;
    int digits[6];
    for (int guess = 138241; guess < 674034; guess++) {
        get_digits(guess, digits);
        if (has_two_adjacent(digits) && is_monotonic(digits)) {
            valid_count_1++;
            if (has_exactly_two_adjacent(digits)) {
                valid_count_2++;
            }
        }
    }

    printf("The number of valid passwords is %d.\n", valid_count_1);
    printf("With the 'larger group' restriction it's %d.\n", valid_count_2);
}

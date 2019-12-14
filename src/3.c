#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUT "inputs/3.txt"
#define INITIAL_LIST_LENGTH 10;

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    Direction direction;
    int distance;
} Move;

typedef struct {
    int x;
    int y;
} Coord;

typedef struct {
    Move *moves;
    int count;
    int capacity;
} MoveList;

typedef struct {
    Coord *coords;
    int count;
    int capacity;
} CoordList;

CoordList *coord_list_init() {
    CoordList *list = malloc(sizeof(CoordList));
    list->count = 0;
    list->capacity = INITIAL_LIST_LENGTH;
    list->coords = malloc(sizeof(Coord) * list->capacity);
    return list;
}

void coord_list_add(CoordList *list, Coord *coord) {
    if (list->count * 2 > list->capacity) {
        list->coords = realloc(list->coords, sizeof(Coord) * list->capacity * 2);
        list->capacity *= 2;
    }

    int idx = list->count++;
    list->coords[idx] = *coord;
}

void coord_list_free(CoordList *list) {
    free(list->coords);
    free(list);
}

MoveList *move_list_init() {
    MoveList *list = malloc(sizeof(MoveList));
    list->count = 0;
    list->capacity = INITIAL_LIST_LENGTH;
    list->moves = malloc(sizeof(Move) * list->capacity);
    return list;
}

void move_list_add(MoveList *list, Move *move) {
    if (list->count * 2 > list->capacity) {
        list->moves = realloc(list->moves, sizeof(Move) * list->capacity * 2);
        list->capacity *= 2;
    }

    int idx = list->count++;
    list->moves[idx] = *move;
}

void move_list_free(MoveList *list) {
    free(list->moves);
    free(list);
}

Direction parse_direction(char direction) {
    if (direction == 'L') {
        return LEFT;
    } else if (direction == 'R') {
        return RIGHT;
    } else if (direction == 'U') {
        return UP;
    } else {
        return DOWN;
    }
}

MoveList *parse_line(FILE *f) {
    MoveList *result = move_list_init();
    char directionChar;
    int distance;
    while (1) {
	   int read = fscanf(f, "%c%d,", &directionChar, &distance);
       if (read <= 0 || directionChar == '\n') {
           break;
       }

       Move move = { parse_direction(directionChar), distance };
       move_list_add(result, &move);
    }

    return result;
}

Coord move(Coord start, Direction direction) {
    Coord result = start;
    switch (direction) {
        case UP:
            result.y--;
            break;
        case DOWN:
            result.y++;
            break;
        case LEFT:
            result.x--;
            break;
        case RIGHT:
            result.x++;
            break;
    }

    return result;
}

CoordList *get_wire_coords(MoveList *moveList) {
    CoordList *coordList = coord_list_init();
    Coord current = { 0, 0 };
    for (int idx = 0; idx < moveList->count; idx++) {
        Move currentMove = moveList->moves[idx];
        for (int step = 0; step < currentMove.distance; step++) {
            current = move(current, currentMove.direction);
            coord_list_add(coordList, &current);
        }
    }

    return coordList;
}

int cmp_coord(const void* coord1, const void* coord2) {
    const Coord *a = (const Coord*) coord1;
    const Coord *b = (const Coord*) coord2;
    if (a->x < b->x) {
        return -1;
    } else if (a->x > b->x) {
        return 1;
    } else {
        return a->y - b->y;
    }
}

void sort_coord_list(CoordList *coords) {
    qsort(coords->coords, coords->count, sizeof(Coord), &cmp_coord);
}

CoordList *get_intersections(CoordList *wireOne, CoordList *wireTwo) {
    // O(n * log(n)) average case to sort the coordinate lists
    sort_coord_list(wireOne);
    sort_coord_list(wireTwo);

    // Linear pass through both coordinate lists to detect any equal values
    CoordList *intersections = coord_list_init();
    int wireOneIdx = 0;
    int wireTwoIdx = 0;
    while (wireOneIdx < wireOne->count && wireTwoIdx < wireTwo->count) {
        int cmp = cmp_coord(&wireOne->coords[wireOneIdx], &wireTwo->coords[wireTwoIdx]);
        if (cmp < 0) {
            wireOneIdx++;
        } else if (cmp > 0) {
            wireTwoIdx++;
        } else {
            coord_list_add(intersections, &wireOne->coords[wireOneIdx]);
            wireOneIdx++;
            wireTwoIdx++;
        }
    }

    return intersections;
}

int get_distance_to_closest_to_origin(CoordList *coordList) {
    int bestDistance = -1;
    for (int idx = 0; idx < coordList->count; idx++ ) {
        Coord *c = &coordList->coords[idx];
        int distance = abs(c->x) + abs(c->y);
        if (bestDistance == -1 || distance < bestDistance) {
            bestDistance = distance;
        }
    }

    return bestDistance;
}

int main(int argv, char **argc) {
    FILE *f = fopen(INPUT, "r");
    MoveList *wireOneMoves = parse_line(f);
    MoveList *wireTwoMoves = parse_line(f);
    printf("Parsed %d moves for wire one and %d for wire two.\n", wireOneMoves->count, wireTwoMoves->count);
    fclose(f);

    CoordList *wireOneCoords = get_wire_coords(wireOneMoves);
    CoordList *wireTwoCoords = get_wire_coords(wireTwoMoves);
    move_list_free(wireOneMoves);
    move_list_free(wireTwoMoves);
    printf("Parsed %d coords for wire one and %d for wire two.\n", wireOneCoords->count, wireTwoCoords->count);

    CoordList *intersections = get_intersections(wireOneCoords, wireTwoCoords);
    printf("Found %d intersections.\n", intersections->count);
    coord_list_free(wireOneCoords);
    coord_list_free(wireTwoCoords);

    int bestDistance = get_distance_to_closest_to_origin(intersections);
    printf("The closest coordinate to the origin is %d away.\n", bestDistance);
    coord_list_free(intersections);

    return 0;
}

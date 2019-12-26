#include "10.h"
#define COORD_LIST_INITIAL_CAPACITY 50

int main(int argc, char **argv) {
    CoordList *asteroids = get_asteroids("./inputs/10.txt");

    Coord bestLocation = get_best_vantage_point(asteroids);
    int visibleAsteroids = get_visible_asteroids(asteroids, &bestLocation);
    printf("The best location is (%d,%d) - %d asteroids can be seen.\n",
            bestLocation.x,
            bestLocation.y,
            visibleAsteroids);

    coord_list_free(asteroids);
}

Coord get_best_vantage_point(const CoordList *asteroids) {
    Coord *bestCoord = 0;
    int bestVisibleCount = -1;

    debug("Checking for best candidates in list of %d asteroids.\n",
            asteroids->count);
    for (Coord *candidate = asteroids->coords;
            candidate < asteroids->coords + asteroids->count;
            candidate++) {
        int visibleCount = get_visible_asteroids(asteroids, candidate);
        debug("Candidate (%d,%d) has %d visible asteroids.\n",
                candidate->x,
                candidate->y,
                visibleCount);
        if (visibleCount > bestVisibleCount) {
            debug("\tThat's a new best score!\n");
            bestVisibleCount = visibleCount;
            bestCoord = candidate;
        }
    }

    return *bestCoord;
}

int get_visible_asteroids(const CoordList *asteroids, const Coord *station) {
    CoordList *offsets = get_offsets(asteroids, station);
    coord_list_sort(offsets);
    int uniqueOffsets = coord_list_count_unique(offsets);
    coord_list_free(offsets);
    return uniqueOffsets;
}

CoordList *get_offsets(const CoordList *coords, const Coord *origin) {
    CoordList *offsets = coord_list_init();
    for (Coord *coord = coords->coords;
            coord < coords->coords + coords->count;
            coord++) {
        Coord offset = coord_get_offset(origin, coord);
        if (offset.x != 0 || offset.y != 0) {
            coord_list_append(offsets, &offset);
        }
    }
    return offsets;
}

void coord_list_sort(CoordList *coords) {
    qsort(coords->coords,
            coords->count,
            sizeof(Coord),
            &coord_cmp_by_angle_for_sort);
}

int coord_list_count_unique(const CoordList *coords) {
    int count = 0;
    Coord *left = &coords->coords[0];
    Coord *right = &coords->coords[1];
    Coord *end = &coords->coords[coords->count];
    while (right < end) {
        if (coord_cmp_by_angle(left, right) == 0) {
            count++;
            while (left < end && coord_cmp_by_angle(left, right) == 0) {
                left++;
            }
            right = left + 1;
        } else {
            count++;
            left++;
            right++;

            if (right == end) {
                count++;
            }
        }
    }
    return count;
}


Coord coord_get_offset(const Coord *from, const Coord *to) {
    Coord result = {to->x - from->x, to->y - from->y};
    return result;
}

int coord_cmp_by_angle(const Coord *a, const Coord *b) {
    double angleA = coord_get_angle(a);
    double angleB = coord_get_angle(b);
    if (fabs(angleA - angleB) < EPSILON) {
        return 0;
    } else if (angleA > angleB) {
        return 1;
    } else {
        return -1;
    }
}

int coord_cmp_by_angle_for_sort(const void *a, const void *b) {
    return coord_cmp_by_angle((const Coord *)a, (const Coord *)b);
}

int sgn(int x) {
    if (x < 0) {
        return -1;
    } else if (x > 0) {
        return 1;
    } else {
        return 0;
    }
}

double coord_get_angle(const Coord *a) {
    double rawAngle = atan((double)a->y / a->x);
    double trueAngle;
    if (sgn(a->x) >= 0) {
        if (sgn(a->y) >= 0) {
            trueAngle = rawAngle;
        } else {
            trueAngle = rawAngle + PI * 2;
        }
    } else {
        trueAngle = rawAngle + PI;
    }

    return trueAngle;
}

CoordList *get_asteroids(const char *path) {
    FILE *f = fopen(path, "r");
    CoordList *coords = coord_list_init();

    int c = 0;
    Coord pos = {0, 0};
    while (c != EOF) {
        c = getc(f);
        switch (c) {
            case '#':
                coord_list_append(coords, &pos);
                // Fall through
            case '.':
                pos.x++;
                break;
            case '\n':
                pos.y++;
                pos.x = 0;
                break;
            default:
                fprintf(stderr, "Unknown character %c in input.\n", c);
        }
    }

    fclose(f);
    return coords;
}

CoordList *coord_list_init() {
    CoordList *coords = malloc(sizeof(CoordList));
    coords->count = 0;
    coords->capacity = COORD_LIST_INITIAL_CAPACITY;
    coords->coords = malloc(coords->capacity * sizeof(Coord));
    return coords;
}

void coord_list_free(CoordList *coords) {
    free(coords->coords);
    free(coords);
}

void coord_list_append(CoordList *coords, const Coord *coord) {
    if (coords->count * 2 > coords->capacity) {
        coords->capacity *= 2;
        coords->coords = realloc(coords->coords, coords->capacity * sizeof(Coord));
    }

    coords->coords[coords->count++] = *coord;
}

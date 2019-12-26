#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG_ENABLE 0
#include <debug.h>

#define PI 3.141592653589
#define EPSILON 0.000001

typedef struct {
    int x;
    int y;
} Coord;

Coord coord_get_offset(const Coord *from, const Coord *to);
int coord_cmp_by_angle(const Coord *, const Coord *);
int coord_cmp_by_angle_for_sort(const void *, const void *);
double coord_get_angle(const Coord *);
int sgn(int);

typedef struct {
    Coord *coords;
    int count;
    int capacity;
} CoordList;

CoordList *get_asteroids(const char *);
CoordList *get_offsets(const CoordList *, const Coord *);

CoordList *coord_list_init();
void coord_list_free(CoordList *);
void coord_list_append(CoordList *, const Coord *);
void coord_list_sort(CoordList *);
int coord_list_count_unique(const CoordList *); // Must be sorted!

int get_visible_asteroids(const CoordList *, const Coord *);
Coord get_best_vantage_point(const CoordList *);

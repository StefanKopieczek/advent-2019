#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define COM 0

typedef struct {
    char a;
    char b;
    char c;
} Label;

bool label_eq(Label *a, Label *b);

typedef struct {
    Label *labels;
    int count;
    int capacity;
} Directory;

Directory *directory_init();
void directory_free(Directory *);
int directory_index(Directory *, Label);

typedef struct {
    Label primary;
    Label satellite;
} OrbitPair;

typedef struct {
    OrbitPair *pairs;
    int count;
    int capacity;
} OrbitList;

OrbitList *orbit_list_init();
void orbit_list_free(OrbitList *);
void orbit_list_add(OrbitList *orbits, Label primary, Label satellite);
OrbitList *orbit_list_parse(FILE *);
void populate_directory(Directory *directory, OrbitList *orbits);
// Adjacency matrix where graph[a][b] is true iff b is a direct satellite of a.
typedef struct {
    bool **nodes;
    int numNodes;
} OrbitGraph;

OrbitGraph *graph_init(int numObjects);
void graph_free(OrbitGraph *);
void graph_add_orbit(OrbitGraph *graph, int primary, int satellite);
bool graph_has_orbit(OrbitGraph *graph, int primary, int satellite);
OrbitGraph *graph_from_list(OrbitList *, Directory *directory);

typedef struct {
    int indirectSatellites;
    int totalChildOrbits;
} OrbitCountResult;

void count_child_orbits(OrbitCountResult *output, OrbitGraph *graph, int node);

int *get_parents(OrbitGraph *graph);
void recursively_add_children_to_parent_map(OrbitGraph *graph, int *parentMap, int current);
int get_orbital_distance(OrbitGraph *graph, int start, int end);

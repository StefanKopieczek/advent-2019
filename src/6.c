#include "6.h"

#define INPUT "./inputs/6.txt"
#define INITIAL_LIST_CAPACITY 20

int main(int argc, char **argv) {
    Directory *directory  = directory_init();

    FILE *f = fopen(INPUT, "r");
    OrbitList *orbits = orbit_list_parse(f);
    fclose(f);
    printf("Found %d orbit pairs in the map.\n", orbits->count);

    OrbitGraph *graph = graph_from_list(orbits, directory);
    orbit_list_free(orbits);

    OrbitCountResult orbitInfo;
    count_child_orbits(&orbitInfo, graph, COM);
    printf("There are %d indirect orbits in this map.\n", orbitInfo.totalChildOrbits);

    Label youLabel = { 'Y', 'O', 'U' };
    Label santaLabel = { 'S', 'A', 'N' };
    int you = directory_index(directory, youLabel);
    int santa = directory_index(directory, santaLabel);
    printf("You: %d, santa: %d, total nodes: %d.\n", you, santa, graph->numNodes);
    int distanceToSanta = get_orbital_distance(graph, you, santa) - 1;
    printf("It would take %d orbital changes to reach Santa.\n", distanceToSanta);
    graph_free(graph);
}

void count_child_orbits(OrbitCountResult *output, OrbitGraph *graph, int primary) {
    output->indirectSatellites = 0;
    output->totalChildOrbits = 0;
    for (int satellite = 0; satellite < graph->numNodes; satellite++) {
        if (graph_has_orbit(graph, primary, satellite)) {
            OrbitCountResult childInfo;
            count_child_orbits(&childInfo, graph, satellite);
            output->indirectSatellites += 1 + childInfo.indirectSatellites;
            output->totalChildOrbits += childInfo.indirectSatellites + childInfo.totalChildOrbits + 1;
        }
    }
}

int get_orbital_distance(OrbitGraph *graph, int start, int end) {
    int *distancesToEnd = malloc(sizeof(int) * graph->numNodes);
    for (int i = 0; i < graph->numNodes; i++) {
        distancesToEnd[i] = -1;
    }

    int *parents = get_parents(graph);

    // Move from the destination to COM, recording the distance travelled at each stage.
    int current = end;
    int distance = -1;
    do {
        distancesToEnd[current] = distance;
        current = parents[current];
        distance++;
    } while (current != COM);

    // Now move from the start up its parents until a parent of the destination is reached.
    distance = 0;
    current = start;
    while (distancesToEnd[current] == -1) {
        current = parents[current];
        distance++;
    }

    // We've reached a common parent of 'start' and 'end'.
    // Add on the distance from this node to the end, and return it.
    distance += distancesToEnd[current];
    free(parents);
    return distance;
}

int *get_parents(OrbitGraph *graph) {
    int *parents = malloc(sizeof(int) * graph->numNodes);
    recursively_add_children_to_parent_map(graph, parents, COM);
    return parents;
}

void recursively_add_children_to_parent_map(OrbitGraph *graph, int *parents, int current) {
    for (int maybeChild = 0; maybeChild < graph->numNodes; maybeChild++) {
        if (graph->nodes[current][maybeChild]) {
            parents[maybeChild] = current;
            recursively_add_children_to_parent_map(graph, parents, maybeChild);
        }
    }
}

OrbitGraph *graph_init(int numObjects) {
    OrbitGraph *graph = malloc(sizeof(OrbitGraph));
    graph->numNodes = numObjects;
    graph->nodes = malloc(numObjects * sizeof(bool *));
    for (int i = 0; i < numObjects; i++) {
        graph->nodes[i] = malloc(numObjects * sizeof(bool));
        for (int j = 0; j < numObjects; j++) {
            graph->nodes[i][j] = false;
        }
    }

    return graph;
}

void graph_free(OrbitGraph *graph) {
    for (int i = 0; i < graph->numNodes; i++) {
        free(graph->nodes[i]);
    }

    free(graph->nodes);
    free(graph);
}

bool graph_has_orbit(OrbitGraph *graph, int primary, int satellite) {
    return graph->nodes[primary][satellite];
}

void graph_add_orbit(OrbitGraph *graph, int primary, int satellite) {
    graph->nodes[primary][satellite] = true;
}

OrbitGraph *graph_from_list(OrbitList *orbits, Directory *directory) {
    populate_directory(directory, orbits);
    OrbitGraph *graph = graph_init(directory->count);
    for (int i = 0; i < orbits->count; i++) {
        int primary = directory_index(directory, orbits->pairs[i].primary);
        int satellite = directory_index(directory, orbits->pairs[i].satellite);
        graph_add_orbit(graph, primary, satellite);
    }

    return graph;
}

OrbitList *orbit_list_parse(FILE *f) {
    OrbitList *orbits = orbit_list_init();
    Label primary;
    Label satellite;
    while (1) {
        int read = fscanf(f, "%c%c%c)%c%c%c ",
                &primary.a, &primary.b, &primary.c,
                &satellite.a, &satellite.b, &satellite.c);
        if (read < 6) {
            break;
        }
        orbit_list_add(orbits, primary, satellite);
    }

    return orbits;
}

OrbitList *orbit_list_init() {
    OrbitList *list = malloc(sizeof(OrbitList));
    list->count = 0;
    list->capacity = INITIAL_LIST_CAPACITY;
    list->pairs = malloc(list->capacity * sizeof(OrbitPair));
    return list;
}

void orbit_list_free(OrbitList *orbits) {
    free(orbits->pairs);
    free(orbits);
}

void orbit_list_add(OrbitList *orbits, Label primary, Label satellite) {
    if (orbits->count * 2 > orbits->capacity) {
        orbits->capacity *= 2;
        orbits->pairs = realloc(orbits->pairs, orbits->capacity * sizeof(OrbitPair));
    }

    OrbitPair *target = &orbits->pairs[orbits->count++];
    target->primary = primary;
    target->satellite = satellite;
}

Directory *directory_init() {
    Directory *directory = malloc(sizeof(Directory));
    directory->count = 0;
    directory->capacity = INITIAL_LIST_CAPACITY;
    directory->labels = malloc(directory->capacity * sizeof(Label));
    Label com = { 'C', 'O', 'M' };
    directory_index(directory, com);
    return directory;
}

void directory_free(Directory *directory) {
    free(directory->labels);
    free(directory);
}

void populate_directory(Directory *directory, OrbitList *orbits) {
    for (int i = 0; i < orbits->count; i++) {
        directory_index(directory, orbits->pairs[i].primary);
        directory_index(directory, orbits->pairs[i].satellite);
    }
}

int directory_index(Directory *directory, Label label) {
    for (int i = 0; i < directory->count; i++) {
        if (label_eq(&directory->labels[i], &label)) {
            return i;
        }
    }

    // The label does not exist in the directory. We need to add it.
    if (directory->count * 2 > directory->capacity) {
        // First, we need to create some more space, as we're approaching capacity.
        directory->capacity *= 2;
        directory->labels = realloc(directory->labels, directory->capacity * sizeof(Label));
    }

    directory->labels[directory->count++] = label;
}

bool label_eq(Label *a, Label *b) {
    return a->a == b->a && a->b == b->b && a->c == b->c;
}

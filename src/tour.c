#include <stdio.h>
#include <stdlib.h>

#include "../include/tour.h"
#include "../include/graph.h"

// --- Create canonical tour ---
int *create_canonical_tour(int n) {
    int *tour = malloc(n * sizeof(int));
    if (!tour) {
        fprintf(stderr, "[ERROR] Failed to allocate memory for tour.\n");
        return NULL;
    }

    for (int i = 0; i < n; i++) tour[i] = i;

    return tour;
}

// --- Compute total tour cost ---
double compute_tour_cost(TSPGraph *graph, const int *tour, int n) {
    double cost = 0.0;
    for (int i = 0; i < n - 1; i++) {
        cost += get_graph_distance(graph, tour[i], tour[i + 1]);
    }
    // Return to starting node
    cost += get_graph_distance(graph, tour[n - 1], tour[0]);
    return cost;
}

// --- Print a tour ---
void print_tour(const int *tour, int n) {
    printf("Tour: ");
    for (int i = 0; i < n; i++) {
        printf("%d", tour[i]);
        if (i < n - 1) printf(" → ");
    }
    printf("\n");
}

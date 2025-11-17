#include <stdio.h>
#include <stdlib.h>

#include "../include/tour.h"
#include "../include/graph.h"

/**
 * @brief Create the canonical tour.
 * @param n number of cities.
 * @return tour array.
 * @return NULL if error.
 */
int *create_canonical_tour(int n) {
    int *tour = malloc((n + 1) * sizeof(int));
    if (!tour){
        fprintf(stderr, "[ERROR] Failed to allocate memory for tour.\n");
        return NULL;
    } 
    for (int i = 0; i < n; i++) tour[i] = i;
    return tour;
}

/**
 * @brief Compute the total cost of a tour.
 * @param graph TSP graph.
 * @param tour tour array.
 * @param n number of cities.
 * @return cost of tour.
 */
double compute_tour_cost(TSPGraph *graph, int *tour, int n) {
    double cost = 0.0;
    for (int i = 0; i < n - 1; i++) {
        cost += get_graph_distance(graph, tour[i], tour[i + 1]);
    }
    cost += get_graph_distance(graph, tour[n - 1], tour[0]);
    return cost;
}

/**
 * @brief print tour.
 * @param tour tour array.
 * @param n number of cities.
 */
void print_tour(int *tour, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", tour[i] + 1);
        if (i < n - 1) printf(", ");
    }
    printf("]");
}

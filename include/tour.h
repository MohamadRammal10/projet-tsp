#ifndef TOUR_H
#define TOUR_H

#include "graph.h"

// Initializes a canonical tour: 0 → 1 → 2 → ... → n-1
int *create_canonical_tour(int n);

// Computes total cost of a tour, including return to start
double compute_tour_cost(TSPGraph *graph, const int *tour, int n);

// Prints a tour (list of node indices)
void print_tour(const int *tour, int n);

#endif // TOUR_H
#include <stdio.h>
#include <stdlib.h>
#include "tsp_io.h"

#ifndef GRAPH_H
#define GRAPH_H

typedef struct{
    int num_nodes;
    double **adj_matrix;
} TSPGraph;

TSPGraph *create_graph(TSPInstance *instance);
void free_graph(TSPGraph *graph);
void print_graph(TSPGraph *graph);

double get_distance(TSPGraph *graph, int i, int j);
double compute_tour_cost(TSPGraph *graph, int *tour, int size);
void print_tour(int *tour, int length);

#endif
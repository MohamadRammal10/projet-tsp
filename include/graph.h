#include <stdio.h>
#include <stdlib.h>
#include "tsp_io.h"

#ifndef GRAPH_H
#define GRAPH_H

typedef struct{
    int num_nodes;
    HalfMatrix *matrix;
} TSPGraph;

TSPGraph *create_graph(TSPInstance *instance);
void free_graph(TSPGraph *graph);
void print_graph(TSPGraph *graph);
double get_graph_distance(TSPGraph *graph, int i, int j);

#endif
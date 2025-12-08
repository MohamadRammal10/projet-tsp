#include <stdio.h>
#include <stdlib.h>

#include "../include/distance.h"
#include "../include/distance_matrix.h"



#ifndef TSP_IO_H
#define TSP_IO_H

typedef struct {
    char name[50];
    char comment[256];
    char type[20];
    int dimension;
    char edge_weight_type[20]; /* Supports only "EUC_2D" , "GEO", "ATT" */
    double **coords;
    double **distances;
    HalfMatrix *half_matrix;
} TSPInstance;

int read_tsplib(const char *filename, TSPInstance *instance);
int read_node_coords(FILE *file, TSPInstance *instance);
int build_distance_matrix(TSPInstance *instance, DistanceFunc dist_func);
double to_radians(double coord);
void free_tsp_instance(TSPInstance *instance);

#endif
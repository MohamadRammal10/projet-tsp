#include <stdio.h>
#include <stdlib.h>

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
} TSPInstance;

int read_tsplib(const char *filename, TSPInstance *instance);
int read_node_coords(FILE *file, TSPInstance *instance);
int build_distance_matrix(TSPInstance *instance, int mode);
double to_radians(double coord);
int geo_distance(TSPInstance *instance);

#endif
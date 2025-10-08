#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[50];
    int dimension;
    char edge_weight_type[20]; /* "EUC_2D, "GEO", "ATT" */
    double **coords; /* [n][2] */
} TSPInstance;

int read_tsplib(const char *filename, TSPInstance *instance);

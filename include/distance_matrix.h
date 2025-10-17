#ifndef DISTANCE_MATRIX_H
#define DISTANCE_MATRIX_H

#include "distance.h"

typedef struct {
    int n;              // number of nodes
    double **data;      // half-matrix
} HalfMatrix;

HalfMatrix *build_half_distance_matrix(double **coords, int n, DistanceFunc dist_func);
double get_distance(const HalfMatrix *matrix, int i, int j);
void free_half_matrix(HalfMatrix *matrix);

#endif

#include <stdio.h>
#include <stdlib.h>
#include "../include/distance_matrix.h"

HalfMatrix *build_half_distance_matrix(double **coords, int n, DistanceFunc dist_func) {
    HalfMatrix *matrix = malloc(sizeof(HalfMatrix));
    if (!matrix) return NULL;

    matrix->n = n;
    matrix->data = malloc(n * sizeof(double *));
    if (!matrix->data) {
        free(matrix);
        return NULL;
    }

    // allocate triangular structure
    for (int i = 0; i < n; i++) {
        matrix->data[i] = malloc(i * sizeof(double)); // row i has i elements
        if (!matrix->data[i]) {
            for (int k = 0; k < i; k++) free(matrix->data[k]);
            free(matrix->data);
            free(matrix);
            return NULL;
        }
    }

    // fill only lower half (j < i)
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            double d = dist_func(coords[i][0], coords[i][1],
                                 coords[j][0], coords[j][1]);
            matrix->data[i][j] = d;
        }
    }

    printf("[INFO] Half-matrix built (%d nodes, %.0f entries)\n", n, n*(n-1)/2.0);
    return matrix;
}

double get_distance(const HalfMatrix *matrix, int i, int j) {
    if (i == j) return 0.0;
    if (j < i)
        return matrix->data[i][j];
    else
        return matrix->data[j][i];
}

void free_half_matrix(HalfMatrix *matrix) {
    if (!matrix) return;
    for (int i = 0; i < matrix->n; i++)
        free(matrix->data[i]);
    free(matrix->data);
    free(matrix);
}

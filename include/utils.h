#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "tsp_io.h"
#include "brute_force.h"

// -----------------------------------------------------------------------------
// Debug macro
// -----------------------------------------------------------------------------
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

// -----------------------------------------------------------------------------
// Function declarations
// -----------------------------------------------------------------------------
void usage(const char *p);
int parse_args(int argc, char *argv[], const char **filename, int *can, int *bf, int *nn);
int canonical_mode(TSPGraph *graph, TSPInstance instance);
void print_final_results(char *method, double temps_cpu, double longueur, int *permutation, int n);
void print_bf_results(BruteForceState *state);

#endif /* UTILS_H */



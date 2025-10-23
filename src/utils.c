#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/utils.h"
#include "../include/tour.h"

/**
 * @brief Print usage information.
 * @param p Program name.
 * ./tsp -h to show this message.
 */
void usage(const char *p) {
    printf("Usage: %s -f <file> [-c] [-m bf]\n", p);
    printf("Options:\n");
    printf("  -h           Show this help message.\n");
    printf("  -f <file>    TSPLIB instance file\n");
    printf("  -c           Compute and print canonical tour\n");
    printf("  -m bf        Run brute-force search and print tour\n");
}

/**
 * @brief Parse command line arguments.
 * @return -1 if an error occurs.
 * @return 0 on success.
 */
int parse_args(int argc, char *argv[], const char **filename, int *do_canonical, int *do_bf){    
    for(int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") && i + 1 < argc) {
            *filename = argv[++i];
        } else if (!strcmp(argv[i], "-c")) {
            *do_canonical = 1;
        } else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            if (!strcmp(argv[i+1], "bf")) { *do_bf = 1; i++; }
            else { usage(argv[0]); return -1;}
        }
        else {
            usage(argv[0]);
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Canonical mode.
 * Calculate the canonical tour.
 * @return -1 if an error occurs
 * @return 0 on success.
 */
int canonical_mode(TSPGraph *graph, TSPInstance instance){
    int n = graph->num_nodes;
    int *tour = malloc((n + 1) * sizeof(int));
    if (!tour) { 
        free_graph(graph);
        free_half_matrix(instance.half_matrix);
        return -1;
    }

    tour[0] = 0;
    for (int i = 1; i < n; i++) tour[i] = i;  /* 0,1,2,...,n-1 */
    tour[n] = 0;                              /* return to start */

    double len = compute_tour_cost(graph, tour, n + 1);

    free(tour);
    return len;
}
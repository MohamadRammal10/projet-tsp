#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/tour.h"
#include "../include/brute_force.h"
#include "../include/utils.h"

int main(int argc, char *argv[]) {
    const char *filename = NULL;
    int can = 0;
    double can_len = 0.0;
    int bf = 0;

    if (parse_args(argc, argv, &filename, &can, &bf) == -1){
        return EXIT_FAILURE;
    }

    /* read file instance */
    TSPInstance instance;
    if (read_tsplib(filename, &instance) != 0) return EXIT_FAILURE;

    /* create graph from instance*/
    TSPGraph *graph = create_graph(&instance);
    if (!graph) { free_half_matrix(instance.half_matrix); return EXIT_FAILURE; }

    /* canonical mode: compute canonical tour */
    if (can) {
        if((can_len = canonical_mode(graph, instance)) == -1) {
            free_graph(graph);
            free_half_matrix(instance.half_matrix);
            return EXIT_FAILURE;
        }
    }

    /* Brute force solution */
    if (bf) {
        run_brute_force_graph(graph, instance.name);
    }

    //TODO : Nearest neighbor algorithm

    //TODO : Random walk algorithm

    //TODO : 2-opt algorithm

    //TODO : Genetic algorithm

    free_graph(graph);
    free_half_matrix(instance.half_matrix);
    printf("Successfully exited.\n");
    return 0;
}
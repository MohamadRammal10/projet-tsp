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
    int do_canonical = 0;
    int do_bf = 0;

    /* ./tsp -h */
    if(argc == 2 && !strcmp(argv[1], "-h")) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (parse_args(argc, argv, &filename, &do_canonical, &do_bf) == -1){
        return EXIT_FAILURE;
    }

    /* read file instance */
    TSPInstance instance;
    if (read_tsplib(filename, &instance) != 0) return EXIT_FAILURE;

    /* create graph from instance*/
    TSPGraph *graph = create_graph(&instance);
    if (!graph) { free_half_matrix(instance.half_matrix); return EXIT_FAILURE; }

    /* canonical mode: compute canonical tour */
    if (do_canonical) {
        if(canonical_mode(graph, instance) == -1) {
            return EXIT_FAILURE;
        }
    }

    /* brute force mode: function prints its own "Tour <name> bf ..." line */
    if (do_bf) {
        run_brute_force_graph(graph, instance.name);
    }

    free_graph(graph);
    free_half_matrix(instance.half_matrix);
    printf("Successfully exited.\n");
    return 0;
}
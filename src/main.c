#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/tour.h"
#include "../include/brute_force.h"

static void usage(const char *p) {
    printf("Usage: %s -f <file> [-c] [-m bf]\n", p);
}

/* BF will print its own:  Tour <instance> bf <secs> <length> [tour] */
int main(int argc, char *argv[]) {
    const char *filename = NULL;
    int do_canonical = 0;
    int do_bf = 0;

    /* parse args */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") && i + 1 < argc) {
            filename = argv[++i];
        } else if (!strcmp(argv[i], "-c")) {
            do_canonical = 1;
        } else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            if (!strcmp(argv[i+1], "bf")) { do_bf = 1; i++; }
            else { usage(argv[0]); return EXIT_FAILURE; }
        } else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (!filename || (!do_canonical && !do_bf)) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* read instance and build graph */
    TSPInstance instance;
    if (read_tsplib(filename, &instance) != 0) return EXIT_FAILURE;
    TSPGraph *graph = create_graph(&instance);
    if (!graph) { free_half_matrix(instance.half_matrix); return EXIT_FAILURE; }

    /* canonical mode: print one line exactly: Tour <name> canonical <secs> <length> [1,...] */
    /* replace the whole canonical block */
if (do_canonical) {
    int n = graph->num_nodes;
    int *tour = malloc((n + 1) * sizeof(int));
    if (!tour) { free_graph(graph); free_half_matrix(instance.half_matrix); return EXIT_FAILURE; }

    tour[0] = 0;
    for (int i = 1; i < n; i++) tour[i] = i;  /* 0,1,2,...,n-1 */
    tour[n] = 0;                              /* return to start */

    clock_t t0 = clock();
    double len = compute_tour_cost(graph, tour, n + 1);
    double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;

    /* exactly what the Python parser expects */
    printf("Tour %s canonical %.3f %.0f [1", instance.name, secs, len);
    for (int i = 1; i < n; i++) printf(",%d", i + 1);  /* no spaces */
    printf("]\n");

    free(tour);
}


    /* brute force mode: function prints its own "Tour <name> bf ..." line */
    if (do_bf) {
        run_brute_force_graph(graph, instance.name);
    }

    free_graph(graph);
    free_half_matrix(instance.half_matrix);
    return 0;
}

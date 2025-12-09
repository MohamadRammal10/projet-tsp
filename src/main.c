#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/tour.h"
#include "../include/brute_force.h"
#include "../include/utils.h"
#include "../include/nearest_neighbor.h"
#include "../include/random_walk.h"
#include "../include/genetic.h"
#include "../include/two_opt.h"

#define POP_SIZE               100
#define NUM_GENERATIONS        1000
#define MUTATION_RATE          0.02


int main(int argc, char *argv[]) {
    const char *filename = NULL;

    int can = 0, bf = 0, nn = 0, rw = 0, ga = 0, gadpx = 0, twooptnn = 0, twooptrw = 0, all = 0;
    int pop_size = POP_SIZE;
    int num_generations = NUM_GENERATIONS;
    double mutation_rate = MUTATION_RATE;

    if (parse_args(argc, argv, &filename, &can, &bf, &nn, &rw, &twooptnn, &twooptrw, &ga, &gadpx, &all, &pop_size, &num_generations, &mutation_rate) == -1){
        return EXIT_FAILURE;
    }

    TSPInstance instance;
    if (read_tsplib(filename, &instance) != 0) return EXIT_FAILURE;

    TSPGraph *graph = create_graph(&instance);
    if (!graph) { 
        free_tsp_instance(&instance);
        free_half_matrix(instance.half_matrix); 
        return EXIT_FAILURE; 
    }

    if (all) {
        nn = 1;
        rw = 1;
        twooptnn = 1;
        twooptrw = 1;
        ga = 1;
        gadpx = 1;
    }

    if (can) {
        double can_len = canonical_mode(graph, instance);
        if (can_len == -1) {
            free_graph(graph);
            free_half_matrix(instance.half_matrix);
            free_tsp_instance(&instance);
            return EXIT_FAILURE;
        }
    }

    if (bf) run_brute_force_graph(graph, instance.name);
    if (nn) run_nearest_neighbor(graph, instance.name);
    if (rw) run_random_walk(graph, instance.name);
    if (twooptnn) run_two_opt_nearest_neighbor(graph, instance.name);
    if (twooptrw) run_two_opt_random_walk(graph, instance.name);
    
    if (ga) { 
        DEBUG_PRINT("Running Genetic Algorithm...\n");
        run_genetic_algorithm(graph, instance.name, pop_size, num_generations, mutation_rate);
    }
    if (gadpx){
        DEBUG_PRINT("Running Genetic Algorithm DPX...\n");
        run_genetic_algorithm_dpx(graph, instance.name, pop_size, num_generations, mutation_rate);
    }
  
    free_graph(graph);
    free_half_matrix(instance.half_matrix);
    free_tsp_instance(&instance);
    
    printf("Successfully exited.\n");
    return 0;
}

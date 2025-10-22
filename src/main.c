#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/tour.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s -f tests/<tsp_file> [-c]\n", prog_name);
    printf("Options:\n");
    printf(" -f <file>    Specify the TSP file to read\n");
    printf(" -c           Compute and print canonical tour length\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3){
        print_usage(argv[0]);  
        return EXIT_FAILURE;
    }
    
    char *filename = NULL;
    int compute_canonical = 0;
    
    // --- Parse command-line args ---
    for (int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-f") == 0 && i + 1 < argc){
            filename = argv[++i];
        } 
        else if (strcmp(argv[i], "-c") == 0) {
            compute_canonical = 1;
        } 
        else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!filename) {
        fprintf(stderr, "[ERROR] Missing file argument (-f <file> required)\n");
        return EXIT_FAILURE;
    }
    
    printf("Projet TSP - initial version\n");
    printf("Reading TSPLIB file: %s\n", filename);

    // --- Load TSP instance ---
    TSPInstance instance;
    if (read_tsplib(filename, &instance) != 0) {
        printf("[ERROR] reading TSP file\n");
        return EXIT_FAILURE;
    }

    // --- Create graph (half matrix)---
    TSPGraph *graph = create_graph(&instance);
    if(!graph){
        printf("[ERROR] Failed to create graph\n");
        free_half_matrix(instance.half_matrix);
        return EXIT_FAILURE;
    }

    print_graph(graph);

    // --- Example tour and cost computation ---
    // TODO: Refactor this into a separate function/module for tour generation
    if (compute_canonical)
    {
        int n = graph->num_nodes;
        int *tour = create_canonical_tour(n);
        if (!tour) {
            fprintf(stderr, "[ERROR] Memory allocation for tour failed\n");
            free_graph(graph);
            free_half_matrix(instance.half_matrix);
            return EXIT_FAILURE;
        }
        
        print_tour(tour,n);

        double total_cost = compute_tour_cost(graph, tour, n);
        printf("Canonical tour cost: %lf\n", total_cost);
        free(tour);
    }
    
    free_graph(graph);
    free_half_matrix(instance.half_matrix);
    printf("[DEBUG] All memory freed. Exiting cleanly.\n");
    return 0;
}
#include <stdio.h>
#include "../include/tsp_io.h"
#include "../include/graph.h"

int main(int argc, char *argv[]) {
    //temp
    if (argc != 2){
        printf("Usage: %s tests/<tsp_file>\n", argv[0]);    
        return -1;
    }
    
    printf("Projet TSP - version initiale\n");    

    TSPInstance instance;
    char *tsp = argv[1]; 
    printf("Reading TSP files %s\n", tsp);
    if (read_tsplib(tsp, &instance) != 0) {
        printf("Error reading TSP file\n");
        return -1;
    }

    TSPGraph *graph = create_graph(&instance);
    if(!graph){
        printf("Error creating graph.\n");
        return -1;
    }

    print_graph(graph);
    int tour[] = {0, 0, 0, 1};
    print_tour(tour, 4);
    double cost = compute_tour_cost(graph, tour, 4);
    printf("Tour cost: %lf\n", cost);

    free_graph(graph);
    return 0;
}
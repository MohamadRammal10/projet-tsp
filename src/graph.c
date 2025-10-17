#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// Create a graph from the TSP instance
TSPGraph *create_graph(TSPInstance *instance){
    TSPGraph *graph = malloc(sizeof(TSPGraph));
    if (!graph) return NULL;

    graph->num_nodes = instance->dimension;
    graph->matrix = instance->half_matrix;

    printf("[DEBUG] Graph created with %d nodes\n", graph->num_nodes);
    return graph;
}

// Free the memory of the graph
void free_graph(TSPGraph *graph){
    if(!graph) return;
    free(graph);
    printf("[DEBUG] Graph memory freed\n");
}

// Print the adjacency matrix of the graph
void print_graph(TSPGraph *graph){
    printf("Graph Adjacency (lower half):\n");
    for(int i = 0; i < graph->num_nodes; i++){
        for(int j = 0; j < i; j++){
            printf("%6.2f ", get_distance(graph->matrix, i, j));
        }
        printf("\n");
    }
}

// Get distance between two cities
double get_graph_distance(TSPGraph *graph, int i, int j) {
    return get_distance(graph->matrix, i, j);
}

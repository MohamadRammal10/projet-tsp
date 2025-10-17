#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// Create a graph from the TSP instance
TSPGraph *create_graph(TSPInstance *instance){
    TSPGraph *graph = malloc(sizeof(TSPGraph));
    if (!graph) return NULL;

    graph->num_nodes = instance->dimension;
    graph->adj_matrix = malloc(graph->num_nodes * sizeof(double *));
    if (!graph->adj_matrix) return NULL;

    for (int i = 0; i < graph->num_nodes; i++) {
        graph->adj_matrix[i] = malloc(graph->num_nodes * sizeof(double));
        if (!graph->adj_matrix[i]) return NULL;

        for (int j = 0; j < graph->num_nodes; j++) {
            graph->adj_matrix[i][j] = instance->distances[i][j];
        }
    }
    printf("[DEBUG] Graph created with %d nodes\n", graph->num_nodes);
    return graph;
}

// Free the memory of the graph
void free_graph(TSPGraph *graph){
    if(!graph) return;
    for(int i = 0; i < graph->num_nodes; i++) {
        free(graph->adj_matrix[i]);
    }
    free(graph->adj_matrix);
    free(graph);
    printf("[DEBUG] Graph memory freed\n");
}

// Print the adjacency matrix of the graph
void print_graph(TSPGraph *graph){
    printf("Graph Adjacency Matrix (%d x %d):\n", graph->num_nodes, graph->num_nodes);
    for(int i = 0; i < graph->num_nodes; i++){
        for(int j = 0; j < graph->num_nodes; j++){
            printf("%8.2f ", graph->adj_matrix[i][j]);
        }
        printf("\n");
    }
}

// Get distance between two cities
double get_distance(TSPGraph *graph, int i, int j) {
    if (!graph || i < 0 || j < 0 || i >= graph->num_nodes || j >= graph->num_nodes) {
        fprintf(stderr, "Invalid indices in get_distance(%d,%d)\n", i, j);
        return -1;
    }
    return graph->adj_matrix[i][j];
}

// Compute total cost of a given tour path
double compute_tour_cost(TSPGraph *graph, int *tour, int size) {
    double total = 0.0;
    for (int i = 0; i < size - 1; i++) {
        total += get_distance(graph, tour[i], tour[i+1]);
    }
    total += get_distance(graph, tour[size - 1], tour[0]); // return to start
    return total;
}

// Print a tour
void print_tour(int *tour, int size) {
    printf("Tour: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", tour[i] + 1);
    }
    printf("→ %d\n", tour[0] + 1);
}
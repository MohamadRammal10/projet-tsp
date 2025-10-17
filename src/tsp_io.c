#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/tsp_io.h"
#include "../include/distance.h"
#include "../include/distance_matrix.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int read_tsplib(const char *filename, TSPInstance *instance) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[256];
    while(fgets(line, sizeof(line), file)) {
        if (sscanf(line, "NAME : %s", instance->name) == 1) {
            printf("[INFO] Name : %s\n", instance->name);
        }

        else if (sscanf(line, "COMMENT : %[^\n]", instance->comment) == 1) {
            printf("[INFO] Comment : %s\n", instance->comment);
        }

        else if (sscanf(line, "TYPE : %s", instance->type) == 1) {
            printf("[INFO] Type : %s\n", instance->type);
        }        
        
        else if (sscanf(line, "DIMENSION : %d", &instance->dimension) == 1) {
            printf("[INFO] Dimension : %d\n", instance->dimension);
        }

        else if (sscanf(line, "EDGE_WEIGHT_TYPE : %s", instance->edge_weight_type) == 1) {
            printf("[INFO] Edge weight type : %s\n", instance->edge_weight_type);
        }

        else if (strncmp(line, "NODE_COORD_SECTION", 18) == 0) {
            if (read_node_coords(file, instance) != 0) {
                fclose(file);
                return -1;
            }
            break; // exit header parsing
        }
    }

    fclose(file);
    
    DistanceFunc dist_func = NULL;

    if (strcmp(instance->edge_weight_type, "EUC_2D") == 0) {
        dist_func = distance_euc_2d;
    } else if (strcmp(instance->edge_weight_type, "ATT") == 0) {
        dist_func = distance_att;
    } else if (strcmp(instance->edge_weight_type, "GEO") == 0)  {
        dist_func = distance_geo;
    } else {
        fprintf(stderr, "Unsupported EDGE_WEIGHT_TYPE: %s\n", instance->edge_weight_type);
        return -1;
    }

    instance->half_matrix = build_half_distance_matrix(instance->coords, instance->dimension, dist_func);
    if (!instance->half_matrix) {
        fprintf(stderr, "Error building half distance matrix\n");
        return -1;
    }

    return 0;
}

int read_node_coords(FILE *file, TSPInstance *instance){
    instance->coords = malloc(instance->dimension * sizeof(double *));
    if (!instance->coords) return -1;

    for (int i = 0; i < instance->dimension; i++) {
        instance->coords[i] = malloc(2 * sizeof(double));
        if (!instance->coords[i]) return -1;
    }

    for (int i = 0; i < instance->dimension; i++) {
        int id;
        double x, y;
        if (fscanf(file, "%d %lf %lf", &id, &x, &y) != 3) {
            fprintf(stderr, "Error reading coordinates for node %d\n", i+1);
            return -1;
        }
        instance->coords[i][0] = x;
        instance->coords[i][1] = y;
        printf("[DEBUG] Read coords: %d -> (%lf, %lf)\n", id, x, y);
    }

    return 0;
}

int build_distance_matrix(TSPInstance *instance,  DistanceFunc dist_func){
    int n = instance->dimension;

    instance->distances = malloc(n * sizeof(double *));
    if (!instance->distances) return -1;

    for (int i = 0; i < n; i++) {
        instance->distances[i] = malloc(n * sizeof(double));
        if (!instance->distances[i]) return -1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j){
                instance->distances[i][j] = 0.0;
                
            } else {
                instance->distances[i][j] = dist_func(
                    instance->coords[i][0], instance->coords[i][1],
                    instance->coords[j][0], instance->coords[j][1]
                );
            }        
        }
    }
    printf("[INFO] Distance matrix built (%d x %d)\n", n, n);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/tsp_io.h"

int read_tsplib(const char *filename, TSPInstance *instance) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[256];
    // parse header
    while(fgets(line, sizeof(line), file)) {
        if (sscanf(line, "NAME : %s", instance->name) == 1) {
            printf("[DEBUG] Name : %s\n", instance->name);
        }

        else if (sscanf(line, "COMMENT : %[^\n]", instance->comment) == 1) {
            printf("[DEBUG] Comment : %s\n", instance->comment);
        }

        else if (sscanf(line, "TYPE : %s", instance->type) == 1) {
            printf("[DEBUG] Type : %s\n", instance->type);
        }        
        
        else if (sscanf(line, "DIMENSION : %d", &instance->dimension) == 1) {
            printf("[DEBUG] Dimension : %d\n", instance->dimension);
        }

        else if (sscanf(line, "EDGE_WEIGHT_TYPE : %s", instance->edge_weight_type) == 1) {
            printf("[DEBUG] Edge weight type : %s\n", instance->edge_weight_type);
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

    if (strcmp(instance->edge_weight_type, "EUC_2D") == 0) {
        build_distance_matrix(instance);
    } else {
        fprintf(stderr, "Unsupported EDGE_WEIGHT_TYPE: %s\n", instance->edge_weight_type);
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

int build_distance_matrix(TSPInstance *instance){
    int n = instance->dimension;
    instance->distances = malloc(n * sizeof(double *));
    if (!instance->distances) return -1;

    for (int i = 0; i < n; i++) {
        instance->distances[i] = malloc(n * sizeof(double));
        if (!instance->distances[i]) return -1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                instance->distances[i][j] = 0.0;
            else {
                double dx = instance->coords[i][0] - instance->coords[j][0];
                double dy = instance->coords[i][1] - instance->coords[j][1];
                instance->distances[i][j] = (int)(sqrt(dx*dx + dy*dy) + 0.5);
            }
        }
    }

    printf("[DEBUG] Distance matrix built (%d x %d)\n", n, n);
    return 0;
}
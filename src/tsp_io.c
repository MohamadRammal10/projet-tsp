#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/tsp_io.h"

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
        build_distance_matrix(instance, 0);
    } else if (strcmp(instance->edge_weight_type, "ATT") == 0) {
        build_distance_matrix(instance, 1);
    } else if (strcmp(instance->edge_weight_type, "GEO") == 0)  {
        build_distance_matrix(instance, 2);
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

/* Mode 0 : EUC_2D ; Mode 1 : ATT ; Mode 2 : GEO*/
int build_distance_matrix(TSPInstance *instance, int mode){
    int n = instance->dimension;
    instance->distances = malloc(n * sizeof(double *));
    if (!instance->distances) return -1;

    for (int i = 0; i < n; i++) {
        instance->distances[i] = malloc(n * sizeof(double));
        if (!instance->distances[i]) return -1;
    }

    if (mode == 2) {  // GEO type
            double *lat = malloc(n * sizeof(double));
            double *lon = malloc(n * sizeof(double));
            if (!lat || !lon) return -1;

            for (int i = 0; i < n; i++) {
                lat[i] = to_radians(instance->coords[i][0]);
                lon[i] = to_radians(instance->coords[i][1]);
            }

            const double RRR = 6378.388;

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i == j)
                        instance->distances[i][j] = 0.0;
                    else {
                        double q1 = cos(lon[i] - lon[j]);
                        double q2 = cos(lat[i] - lat[j]);
                        double q3 = cos(lat[i] + lat[j]);
                        instance->distances[i][j] = (int)(
                            RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0
                        );
                    }
                }
            }

            free(lat);
            free(lon);
            printf("[DEBUG] GEO distance matrix built (%d x %d)\n", n, n);
            return 0;
        }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j){
                instance->distances[i][j] = 0.0;
                continue;
            }
            
            double dx = instance->coords[i][0] - instance->coords[j][0];
            double dy = instance->coords[i][1] - instance->coords[j][1];

            if (mode == 1) { // ATT
                double rij = sqrt((dx * dx + dy * dy) / 10.0);
                int tij = (int)(rij + 0.5);
                instance->distances[i][j] = (tij < rij) ? tij + 1 : tij;
            }

            else if (mode == 0) { // EUC_2D
                instance->distances[i][j] = (int)(sqrt(dx * dx + dy * dy) + 0.5);
            }            
        }
    }
    printf("[DEBUG] Distance matrix built (%d x %d)\n", n, n);
    return 0;
}

double to_radians(double coord) {
    int deg = (int)coord;
    double min = coord - deg;
    return M_PI * (deg + 5.0 * min / 3.0) / 180.0;
}
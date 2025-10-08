#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tsp_io.h"

/* written by fourat */
int read_tsplib(const char *filename, TSPInstance *instance) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[256];
    // parse header
    while(fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "NAME : %s", instance->name) == 1) {
            printf("[DEBUG] Found name: %s\n", instance->name);
        }

        if (sscanf(line, "DIMENSION : %d", &instance->dimension) == 1) {
            printf("[DEBUG] Found dimension: %d\n", instance->dimension);
        }

        if (sscanf(line, "EDGE_WEIGHT_TYPE: %s", instance->edge_weight_type) == 1) {
            printf("[DEBUG] Found edge weight type: %s\n", instance->edge_weight_type);
        }
        
        // allocate memory for coords
        instance->coords = malloc(instance->dimension * sizeof(double *));
        for(int i = 0; i < instance->dimension; i++) {
            instance->coords[i] = malloc(2 * sizeof(double));
        }

        // parse coords
        if (strncmp(line, "NODE_COORD_SECTION", 18) == 0) {
            instance->coords = malloc(instance->dimension * sizeof(double*));
            for (int i = 0; i < instance->dimension; i++)
                instance->coords[i] = malloc(2 * sizeof(double));

            for (int i = 0; i < instance->dimension; i++) {
                int id;
                double x, y;
                if (fscanf(file, "%d %lf %lf", &id, &x, &y) == -1) {
                    printf("Error reading coordinates\n");
                    fclose(file);
                    return -1;
                }
                instance->coords[i][0] = x;
                instance->coords[i][1] = y;
                printf("[DEBUG] Read coords: %d -> (%lf, %lf)\n", id, x, y);
            }
            break; 
        }
    }
    fclose(file);
    return 0;
}
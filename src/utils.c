#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/tsp_io.h"
#include "../include/graph.h"
#include "../include/utils.h"
#include "../include/tour.h"
#include "../include/brute_force.h"

/**
 * @brief Print usage information.
 * @param p Program name.
 * ./tsp -h to show this message.
 */
void usage(const char *p) {
    printf("-> Make options :\n");
    printf("  make              Compile project\n");
    printf("  make debug        Compile project + debug mode\n");
    printf("  make clean        Clean .o files\n");
    printf("\n");

    printf("Usage: %s -f <file> [-c] [-m <mode>] [-d]\n", p);

    printf("\n");

    printf("-> Options :\n");
    printf("  -h           Show this help message.\n");
    printf("  -f <file>    TSPLIB instance file\n");
    printf("  -c           Compute and print canonical tour\n");
    printf("  -m bf        Run brute-force search and print tour\n");
}

/**
 * @brief Parse command line arguments.
 * @return -1 if an error occurs.
 * @return 0 on success.
 */
int parse_args(int argc, char *argv[], const char **filename, int *can, int *bf){ 
    if((argc == 2 && !strcmp(argv[1], "-h")) || argc == 1 ) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (!filename) { 
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    for(int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") && i + 1 < argc) {
            *filename = argv[++i];
        } 
        else if (!strcmp(argv[i], "-c")) {
            *can = 1;
        }
        else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            if (!strcmp(argv[i+1], "bf")) { 
                *bf = 1;
                i++;
            } else { 
                printf("Specify the algorithm after -m.\n");
                usage(argv[0]);
                return -1;
            }
            //TODO: next modes : nn, rw, 2opt, ga
        } else {
            usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

/**
 * @brief Canonical mode.
 * Calculate the canonical tour.
 * @param graph TSP graph.
 * @param instance TSP instance.
 * @return -1 if an error occurs
 * @return 0 on success.
 */
int canonical_mode(TSPGraph *graph, TSPInstance instance){
    if (!graph) return -1;
    int n = graph->num_nodes;
    if (n <= 0) return -1;

    /* city 1 -> city 2 -> ... -> city 1 [0,1,2,...,n-1,0] */
    int *tour = create_canonical_tour(n);
    if (!tour) return -1;

    clock_t t0 = clock();
    double len = compute_tour_cost(graph, tour, n + 1);
    double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;

    printf("Instance ; Méthode ; Temps CPU (sec) ; Longueur ; Tour\n");

    printf("%s ; canonical ; %.2f ; %.2f ;", instance.name, secs, len);
    print_tour(tour, n);
    printf("\n");
    free(tour);
    return len;
}

/**
 * @brief Affiche les résultats finaux au format CSV normalisé.
 * TODO: Adapter la fonction pour tout type de méthode (ici seulement "bf") 
 */
void print_final_results(BruteForceState *state) {
    double total_time = (double)(clock() - state->start_time) / CLOCKS_PER_SEC;
    printf("Instance ; Methode ; Temps CPU (sec) ; Meilleure longueur ; Pire longueur ; Tour optimale ; Pire tournée\n");

    // TODO: ADAPT TO ALL METHODS "bf"
    printf("%s ; bf ; %.3f ; %.12f ; %.12f ; ", 
           state->instance_name, total_time, state->best_len, state->worst_len);
    
    print_tour(state->best, state->size);
    printf(" ; ");
    print_tour(state->worst, state->size);
    printf("\n");
}
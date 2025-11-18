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
    printf("  -c           Run canonical search\n");
    printf("  -m bf        Run brute-force search\n");
    printf("  -m nn        Run nearest-neighbor search\n");
    printf("  -m rw        Run random walk search\n");
    printf("  -m 2optnn    Run 2-opt nearest-neighbor search\n");
    printf("  -m 2optrw    Run 2-opt random walk search\n");
    printf("  -m ga <pop_size> <num_generations> <mutation_rate>       Test genetic algorithm mode\n");


    printf("\n");
    printf(" -> Testing config :\n");
    printf("   Usage: python3 test_tsp_c.py\n");
    printf("   -c          Test canonical mode\n");
    printf("   bf          Test brute force mode\n");
    printf("   nn          Test nearest neighbor mode\n");
    printf("   rw          Test random walk mode\n");
    printf("   2optnn      Test 2-opt nearest-neighbor mode\n");
    printf("   2optrw      Test 2-opt random walk mode\n");
    printf("   ga          Test genetic algorithm mode\n");
}

/**
 * @brief Parse command line arguments.
 * @return -1 if an error occurs.
 * @return 0 on success.
 */

int parse_args(int argc, char *argv[], const char **filename, int *can, int *bf, int *nn, int *rw, int *twooptnn, int *twooptrw, int *ga, int *pop_size, int *num_generations, double *mutation_rate){ 
    if((argc == 2 && !strcmp(argv[1], "-h")) || argc == 1 || argc > 8) {
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
        } else if (!strcmp(argv[i], "-m") && i + 1 < argc) {
            const char *m = argv[++i];
            if (!strcmp(m, "bf")) {
                *bf = 1;
            } else if (!strcmp(m, "nn")) {
                *nn = 1;
            } else if (!strcmp(m, "rw")) {
                *rw = 1;
            } else if (!strcmp(m, "ga")) {
                *ga = 1;
                if (i + 3 < argc) {
                *pop_size = atoi(argv[++i]);
                *num_generations = atoi(argv[++i]);
                *mutation_rate = atof(argv[++i]);
                }
                return 0;
            } else if (!strcmp(m, "2optnn")) {
                *twooptnn = 1;
            } else if (!strcmp(m, "2optrw")) {
                *twooptrw = 1;
            } else {
                fprintf(stderr, "Unknown method '%s'\n", m);
                usage(argv[0]);
                return -1;
            }
        } else {
            usage(argv[0]);
            return -1;

        }
    }
    DEBUG_PRINT("PARSE ARGS exit success\n");
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
    printf("[1");
    for (int i = 1; i < n; i++) {
        printf(", %d", tour[i]+1);
    }
    printf("]\n");
    free(tour);
    return len;
}

/**
 * @brief Affiche les résultats finaux au format CSV normalisé.
 */
void print_final_results(char *method, double temps_cpu, double longueur, int *permutation, int n) {
    printf("Instance ; Methode ; Temps CPU (sec) ; Longueur ; Tour\n");
    printf("%s ; nn ; %.3f ; %.12f ; ", method, temps_cpu, longueur);
    print_tour(permutation, n - 1);  // Tour
    printf("\n");
}

void print_bf_results(BruteForceState *state) {
    double total_time = (double)(clock() - state->start_time) / CLOCKS_PER_SEC;
    printf("Instance ; Methode ; Temps CPU (sec) ; Meilleure longueur ; Pire longueur ; Tour optimale ; Pire tournée\n");

    printf("%s ; bf ; %.3f ; %.12f ; %.12f ; ", 
           state->instance_name, total_time, state->best_len, state->worst_len);
    
    print_tour(state->best, state->size);
    printf(" ; ");
    print_tour(state->worst, state->size);
    printf("\n");
}
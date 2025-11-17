#include "../include/two_opt.h"
#include "../include/distance_matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Fonction pour inverser un segment de la tournée
static void reverse_segment(int *tour, int i, int j) {
    while (i < j) {
        int tmp = tour[i];
        tour[i] = tour[j];
        tour[j] = tmp;
        i++;
        j--;
    }
}

// Fonction pour calculer la longeur de la tournée
static double compute_cost(TSPGraph *graph, int *tour, int n) {
    double total = 0.0;
    for (int i = 0; i < n - 1; i++)
        total += get_distance(graph->matrix, tour[i], tour[i + 1]);

    total += get_distance(graph->matrix, tour[n - 1], tour[0]); // retour
    return total;
}

// Fonction d'optimisation avec 2-opt
double two_opt_optimize(TSPGraph *graph, int *tour, int n) {

    double best_cost = compute_cost(graph, tour, n);
    int improved = 1;

    // Tant que la tournée s'améliore essayer des autres permutations
    while (improved) {
        improved = 0;

        for (int i = 1; i < n - 2; i++) {
            for (int j = i + 1; j < n - 1; j++) {

                double before = 
                    get_distance(graph->matrix, tour[i - 1], tour[i]) +
                    get_distance(graph->matrix, tour[j], tour[(j + 1) % n]);

                double after =
                    get_distance(graph->matrix, tour[i - 1], tour[j]) +
                    get_distance(graph->matrix, tour[i], tour[(j + 1) % n]);

                // Si la nouvelle tournée est meilleure, inverser le segment et recommencer l'algo (improved=1)
                if (after < before) {
                    reverse_segment(tour, i, j);
                    best_cost -= (before - after);
                    improved = 1;
                }
            }
        }
    }

    return best_cost;
}

/* -------------------- 2-opt Random Walk -------------------- */
void run_two_opt_random_walk(TSPGraph *graph, const char *instance_name) {

    int n = graph->num_nodes;
    int *tour = malloc(n * sizeof(int));

    // Random Walk
    for (int i = 0; i < n; i++) tour[i] = i;

    srand(time(NULL));
    for (int i = n - 1; i > 1; i--) {
        int j = 1 + rand() % (i - 1);
        int tmp = tour[i];
        tour[i] = tour[j];
        tour[j] = tmp;
    }

    // ----- 2-OPT -----
    clock_t debut = clock();
    double new_cost = two_opt_optimize(graph, tour, n);
    double temps_cpu = (double)(clock() - debut) / CLOCKS_PER_SEC;

    // ----- AFFICHAGE -----
    printf("Instance ; Méthode ; Temps CPU (sec) ; longueur ; Tour\n");
    printf("%s ; 2optrw ; %.3f ; %.12f ; [", instance_name, temps_cpu, new_cost);

    for (int i = 0; i < n; i++) {
        printf("%d", tour[i] + 1);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");

    free(tour);
}

/* -------------------- 2-opt Nearest Neighbor -------------------- */
void run_two_opt_nearest_neighbor(TSPGraph *graph, const char *instance_name) {

    int n = graph->num_nodes;

    int *tour = malloc(n * sizeof(int));
    int *visited = calloc(n, sizeof(int));

    if (!tour || !visited) {
        fprintf(stderr, "Erreur allocation NN-2opt.\n");
        return;
    }

    // ----- NEAREST NEIGHBOR -----
    tour[0] = 0;
    visited[0] = 1;

    for (int i = 1; i < n; i++) {
        int prev = tour[i - 1];
        double best = 1e18;
        int best_j = -1;

        for (int j = 0; j < n; j++) {
            if (!visited[j]) {
                double d = get_distance(graph->matrix, prev, j);
                if (d < best) {
                    best = d;
                    best_j = j;
                }
            }
        }

        tour[i] = best_j;
        visited[best_j] = 1;
    }

    // ----- 2-OPT -----
    clock_t debut = clock();
    double new_cost = two_opt_optimize(graph, tour, n);
    double temps_cpu = (double)(clock() - debut) / CLOCKS_PER_SEC;

    // ----- AFFICHAGE -----
    printf("Instance ; Méthode ; Temps CPU (sec) ; longueur ; Tour\n");
    printf("%s ; 2optnn ; %.3f ; %.12f ; [", instance_name, temps_cpu, new_cost);

    for (int i = 0; i < n; i++) {
        printf("%d", tour[i] + 1);
        if (i < n - 1) printf(", ");
    }

    printf("]\n");

    free(tour);
    free(visited);
}


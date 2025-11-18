#include "../include/random_walk.h"
#include "../include/distance_matrix.h"
#include "../include/tour.h"
#include "../include/utils.h"
#include <time.h>

int *random_walk_optimize(TSPGraph *graph) {

    if (!graph || graph->num_nodes <= 1) {
        fprintf(stderr, "Graphe invalide !\n");
        return NULL;
    }

    int n = graph->num_nodes;
    int *tour = malloc(n * sizeof(int));
    if (!tour) {
        fprintf(stderr, "Erreur de l'allocation du tour\n");
        return NULL;
    }

    // Créer une tournée canonique
    for (int i = 0; i < n; i++) tour[i] = i;

    srand(time(NULL));

    // Mélange aléatoire
    int j, tmp;
    for (int i = n - 1; i > 1; i--) {
        j = 1 + rand() % (i - 1);
        tmp = tour[i];
        tour[i] = tour[j];
        tour[j] = tmp;
    }

    return tour;
}

void run_random_walk(TSPGraph *graph, const char *instance_name) {

    int n = graph->num_nodes;

    clock_t debut = clock();
    int *tour = random_walk_optimize(graph);
    double temps_cpu = (double) (clock() - debut) / CLOCKS_PER_SEC; 

    // Calcul de la distance
    double total = compute_tour_cost(graph, tour, n);

    print_final_results((char *)instance_name, "rw", temps_cpu, total, tour, n);

    free(tour);

}

#include "../include/random_walk.h"
#include "../include/distance_matrix.h"
#include "../include/tour.h"
#include <time.h>

void run_random_walk(TSPGraph *graph, const char *instance_name) {

    if (!graph || graph->num_nodes <= 1) {
        fprintf(stderr, "Graphe invalide !\n");
        return;
    }

    int n = graph->num_nodes;
    int *tour = malloc(n * sizeof(int));
    if (!tour) {
        fprintf(stderr, "Erreur de l'allocation du tour\n");
        return;
    }

    clock_t debut = clock();

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

    // Calcul du temps d'exécution
    double temps_cpu = (double) (clock() - debut) / CLOCKS_PER_SEC; 

    // Calcul de la distance
    double total = compute_tour_cost(graph, tour, n);

    // Affichage du résultat
    fprintf(stdout, "Instance ; Méthode ; Temps CPU (sec) ; longeur ; Tour\n");
    fprintf(stdout, "%s ; rw ; %.2f ; %.2f ; [", instance_name, temps_cpu, total);
    for (int i = 0; i < n; i++) {
        fprintf(stdout, "%d", tour[i] + 1);
        if (i < n - 1) fprintf(stdout, ",");
    }
    fprintf(stdout, "]\n");

    free(tour);
}

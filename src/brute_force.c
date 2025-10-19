#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <signal.h>

#include "../include/brute_force.h"
#include "../include/tour.h"


//  intercepter Ctrl+C 
static volatile sig_atomic_t stop_flag = 0;
static void sigint_handler(int s) { (void)s; stop_flag = 1; }

// permutation ordre lexicographique
static int next_permutation(int *array, int length) {
    int i = length - 2;
    while (i >= 0 && array[i] >= array[i + 1]) {
        i--;
    }
    if (i < 0) return 0;
    int j = length - 1;
    while (array[j] <= array[i]) {
        j--;
    }

    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
    int left = i + 1;
    int right = length - 1;
    while (left < right) {
        temp = array[left];
        array[left] = array[right];
        array[right] = temp;
        left++;
        right--;
    }
    return 1;
}

void run_brute_force_graph(TSPGraph *graph, const char *instance_name) {
    if (!graph) return;
    signal(SIGINT, sigint_handler);  /* gérer Ctrl+C */

    int n = graph->num_nodes;
    if (n <= 1) return;
    int size = n - 1; /* on fixe la ville 0 */

    /* allocations */
    int *perm = malloc(size * sizeof(int));
    int *best = malloc(size * sizeof(int));
    int *worst = malloc(size * sizeof(int));
    int *tour = malloc((n + 1) * sizeof(int));
    if (!perm || !best || !worst || !tour) { perror("malloc"); exit(1); }

    /* initialisation [1,2,...,n-1] */
    for (int i = 0; i < size; i++) {
        perm[i] = i + 1;
        best[i] = perm[i];
        worst[i] = perm[i];
    }

    double best_len = DBL_MAX;
    double worst_len = -DBL_MAX;
    unsigned long long iterations = 0;
    clock_t start = clock();

    int finished = 0;
    while (!finished) {
        /* construire tour [0, perm..., 0] */
        tour[0] = 0;
        for (int i = 0; i < size; i++) tour[i + 1] = perm[i];
        tour[n] = 0;

        double len = compute_tour_cost(graph, tour, n + 1);
        iterations++;

        if (len < best_len) {
            best_len = len;
            for (int i = 0; i < size; i++) best[i] = perm[i];
        }
        if (len > worst_len) {
            worst_len = len;
            for (int i = 0; i < size; i++) worst[i] = perm[i];
        }

        // Ctrl+C 
        if (stop_flag) {
            stop_flag = 0;
            double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;

            printf("\n--- Interruption (Ctrl+C) ---\n");
            printf("Permutation courante (0-based) : [");
            for (int i = 0; i < size; i++) { if (i) printf(", "); printf("%d", perm[i]); }
            printf("]\n");
            printf("Tournée courante (1-based) : [1");
            for (int i = 0; i < size; i++) printf(", %d", perm[i] + 1);
            printf("]\n");

            printf("Meilleure longueur : %.0f | Meilleure tournée : [1", best_len);
            for (int i = 0; i < size; i++) printf(", %d", best[i] + 1);
            printf("]\n");

            printf("Pire longueur : %.0f | Pire tournée : [1", worst_len);
            for (int i = 0; i < size; i++) printf(", %d", worst[i] + 1);
            printf("]\n");

            printf("Itérations : %llu | Temps CPU : %.3fs\n", iterations, elapsed);
            printf("Voulez-vous quitter ? [y/n] ");
            int c = getchar();
            while (c == '\n') c = getchar();
            if (c == 'y' || c == 'Y') {
                printf("Sortie demandée.\n");
                free(perm); free(best); free(worst); free(tour);
                return;
            }
            while (c != '\n' && c != EOF) c = getchar();
            signal(SIGINT, sigint_handler);
            printf("Reprise du calcul...\n");
        }

        /* prochaine permutation ou fin */
        if (!next_permutation(perm, size)) finished = 1;
    }

    double total_time = (double)(clock() - start) / CLOCKS_PER_SEC;


    static int header_done = 0;
    if (!header_done) {
        printf("Instance ; Méthode ; Temps CPU (sec) ; Meilleure longueur ; Pire longueur ; Tour optimale ; Pire tournée\n");
        header_done = 1;
    }

    printf("%s ; bf ; %.3f ; %.0f ; %.0f ; [1", instance_name, total_time, best_len, worst_len);
    for (int i = 0; i < size; i++) printf(", %d", best[i] + 1);
    printf("] ; [1");
    for (int i = 0; i < size; i++) printf(", %d", worst[i] + 1);
    printf("]\n");

    free(perm);
    free(best);
    free(worst);
    free(tour);

}

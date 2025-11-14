#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include "graph.h"
#include <time.h>

/* --- Structure de données pour l'état de l'algorithme --- */

/**
 * @brief Contient toutes les données nécessaires à l'exécution 
 * de l'algorithme de force brute.
 */
typedef struct {
    TSPGraph *graph;
    int n;           // Nombre total de nœuds
    int size;        // Taille de la permutation (n - 1)

    int *perm;       // Permutation courante [1, ..., n-1]
    int *best;       // Meilleure permutation trouvée
    int *worst;      // Pire permutation trouvée
    int *tour;       // Tampon pour le tour complet [0, perm..., 0]

    double best_len;
    double worst_len;
    unsigned long long iterations;
    clock_t start_time;

    const char *instance_name;
} BruteForceState;

void run_brute_force_graph(TSPGraph *graph, const char *instance_name);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <signal.h>
#include <math.h>

#include "../include/utils.h"
#include "../include/tour.h"
#include "../include/brute_force.h"

/* ================================================================
 *  Algorithme de force brute (Brute Force)
 * ================================================================ */

/* --- Variables globales pour la gestion du signal --- */
static volatile sig_atomic_t stop_flag = 0;

/**
 * @brief Gestionnaire de signal pour SIGINT (Ctrl+C).
 * 
 * @param s Numéro du signal reçu.
 */
static void sigint_handler(int s) {
    (void)s;
    stop_flag = 1;
}

/**
 * @brief Génère la prochaine permutation dans l'ordre lexicographique.
 * 
 * Cette fonction implémente l'algorithme de génération de permutations
 * dans l'ordre lexicographique croissant.
 * 
 * @param tour Tableau représentant la permutation courante.
 * @param n Taille du tableau.
 * @return 1 s'il existe une prochaine permutation, 0 sinon.
 */
static int next_permutation(int *tour, int n) {
    // trouver le plus grand indice i tel que tour[i] < tour[i+1]
    int i = n - 2;
    while (i >= 0 && tour[i] >= tour[i + 1]) {
        i--;
    }
    // si n'existe, on a atteint la dernière permutation
    if (i < 0) {
        return 0;
    }
    
    // trouver le plus grand indice j tel que tour[i] < tour[j]
    int j = n - 1;
    while (tour[i] >= tour[j]) {
        j--;
    }
    
    // échanger tour[i] et tour[j]
    int temp = tour[i];
    tour[i] = tour[j];
    tour[j] = temp;
    
    // inverser la sous-séquence de i+1 jusqu'à la fin
    int left = i + 1;
    int right = n - 1;
    while (left < right) {
        temp = tour[left];
        tour[left] = tour[right];
        tour[right] = temp;
        left++;
        right--;
    }
    return 1;
}

/**
 * @brief Résout le TSP par force brute en testant toutes les permutations.
 * 
 * Cette fonction explore toutes les permutations possibles des villes,
 * calcule la longueur de chaque tournée, et garde en mémoire la meilleure
 * et la pire tournée trouvées.
 * 
 * @param graph Pointeur vers le graphe du TSP.
 * @param instance_name Nom de l'instance (pour affichage).
 */
void brute_force(TSPGraph *graph, const char *instance_name) {
    int n = graph->num_nodes;  // nombre total de villes
    
    // allocation des tableaux pour les tournées
    int *current_tour = (int*)malloc(n * sizeof(int));
    int *best_tour = (int*)malloc(n * sizeof(int));
    int *worst_tour = (int*)malloc(n * sizeof(int));
    
    if (!current_tour || !best_tour || !worst_tour) {
        perror("malloc tours");
        free(current_tour);
        free(best_tour);
        free(worst_tour);
        return;
    }
    
    for (int i = 0; i < n; i++) {
        current_tour[i] = i;
    }
    
    double best_length = INFINITY;     // meilleure longueur trouvée
    double worst_length = -INFINITY;   // pire longueur trouvée
    unsigned long long iterations = 0; // compteur d'itérations
    
    clock_t start_time = clock();
    
    do {
        // Calculer la longueur de la tournée courante
        double length = compute_tour_cost(graph, current_tour, n);
        iterations++;
        
        // Mettre à jour la meilleure tournée si nécessaire
        if (length < best_length) {
            best_length = length;
            memcpy(best_tour, current_tour, n * sizeof(int));
        }
        
        // Mettre à jour la pire tournée si nécessaire
        if (length > worst_length) {
            worst_length = length;
            memcpy(worst_tour, current_tour, n * sizeof(int));
        }
        
        // Gérer une éventuelle interruption (Ctrl+C)
        if (stop_flag) {
            stop_flag = 0;
            double elapsed = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            
            printf("\n--- Interruption (Ctrl+C) ---\n");
            printf("Tour courant : ");
            print_tour(current_tour, n);
            printf("\n");
            printf("Meilleure longueur : %.8f | Meilleur tour : ", best_length);
            print_tour(best_tour, n);
            printf("\n");
            printf("Itérations : %llu | Temps CPU : %.3fs\n", iterations, elapsed);
            
            printf("Voulez-vous quitter ? [y/n] ");
            int c = getchar();
            while (c == '\n') c = getchar();
            
            if (c == 'y' || c == 'Y') {
                while (getchar() != '\n');  
                printf("Sortie demandée.\n");
                break;
            } else {
                while (getchar() != '\n');  
                signal(SIGINT, sigint_handler);
                printf("Reprise du calcul...\n");
            }
        }
        
    } while (next_permutation(current_tour+1, n-1));  
    
    // Afficher les résultats finaux
    double total_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    
    printf("Instance ; Methode ; Temps CPU (sec) ; Meilleure longueur ; Pire longueur ; Tour optimale ; Pire tournée\n");
    printf("%s ; bf ; %.3f ; %.12f ; %.12f ; ", 
           instance_name, total_time, best_length, worst_length);
    print_tour(best_tour, n);
    printf(" ; ");
    print_tour(worst_tour, n);
    printf("\n");
    
    // Libérer la mémoire allouée
    free(current_tour);
    free(best_tour);
    free(worst_tour);
}

/**
 * @brief Point d'entrée pour exécuter l'algorithme de force brute sur un graphe.
 * 
 * @param graph Pointeur vers le graphe du TSP.
 * @param instance_name Nom de l'instance (pour affichage).
 */
void run_brute_force_graph(TSPGraph *graph, const char *instance_name) {
    if (!graph) return;
    if (graph->num_nodes <= 1) return;

    signal(SIGINT, sigint_handler);
    brute_force(graph, instance_name);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <signal.h>

#include "../include/utils.h"
#include "../include/tour.h"
#include "../include/brute_force.h"

/* --- Variables globales pour la gestion du signal --- */

/**
 * @brief Drapeau (flag) pour la gestion de l'interruption (Ctrl+C).
 * 
 * 'volatile' empêche le compilateur d'optimiser les accès.
 * 'sig_atomic_t' garantit des écritures/lectures atomiques.
 */
static volatile sig_atomic_t stop_flag = 0;

/**
 * @brief Gestionnaire de signal pour SIGINT (Ctrl+C).
 * Ne fait que lever le drapeau d'arrêt.
 */
static void sigint_handler(int s) {
    (void)s; // Évite l'avertissement "unused parameter"
    stop_flag = 1;
}

/**
 * @brief Calcule la permutation suivante dans l'ordre lexicographique.
 * @return 1 si une nouvelle permutation a été trouvée, 0 si c'était la dernière.
 */
static int next_permutation(int *array, int length) {
    // 1. Trouver le plus grand indice i t.q. array[i] < array[i+1]
    int i = length - 2;
    while (i >= 0 && array[i] >= array[i + 1]) {
        i--;
    }
    if (i < 0) return 0; // C'était la dernière permutation

    // 2. Trouver le plus grand indice j > i t.q. array[j] > array[i]
    int j = length - 1;
    while (array[j] <= array[i]) {
        j--;
    }

    // 3. Échanger array[i] et array[j]
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;

    // 4. Inverser la sous-séquence de i+1 à la fin
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

/* --- Fonctions de gestion de l'état --- */

/**
 * @brief Libère toute la mémoire allouée pour l'état.
 * (Déplacée avant 'create' car 'create' l'utilise en cas d'erreur)
 */
static void free_brute_force_state(BruteForceState *state) {
    if (state) {
        free(state->perm);
        free(state->best);
        free(state->worst);
        free(state->tour);
        free(state);
    }
}

/**
 * @brief Initialise les champs et alloue les tampons pour la structure d'état.
 * @return 1 en cas de succès, 0 en cas d'échec d'allocation.
 */
static int init_brute_force_state(BruteForceState *state, TSPGraph *graph, const char *instance_name) {
    state->graph = graph;
    state->n = graph->num_nodes;
    state->size = state->n - 1; // On fixe la ville 0
    state->instance_name = instance_name;

    // Initialiser les pointeurs à NULL garantit que free_brute_force_state
    // est sûr même en cas d'échec d'allocation partiel.
    state->perm = NULL;
    state->best = NULL;
    state->worst = NULL;
    state->tour = NULL;

    // Allocations des tableaux
    state->perm = malloc(state->size * sizeof(int));
    state->best = malloc(state->size * sizeof(int));
    state->worst = malloc(state->size * sizeof(int));
    state->tour = malloc((state->n + 1) * sizeof(int)); // Tour complet [0, ..., 0]

    if (!state->perm || !state->best || !state->worst || !state->tour) {
        perror("malloc state arrays");
        return 0; // Échec
    }

    // Initialisation
    for (int i = 0; i < state->size; i++) {
        state->perm[i] = i + 1;  // perm = [1, 2, ..., n-1]
        state->best[i] = state->perm[i];
        state->worst[i] = state->perm[i];
    }

    state->best_len = DBL_MAX;
    state->worst_len = -DBL_MAX;
    state->iterations = 0;
    state->start_time = clock();

    return 1; // Succès
}

/**
 * @brief Alloue et initialise la structure d'état.
 */
static BruteForceState *create_brute_force_state(TSPGraph *graph, const char *instance_name) {
    BruteForceState *state = malloc(sizeof(BruteForceState));
    if (!state) {
        perror("malloc BruteForceState");
        return NULL;
    }

    // Appel de la nouvelle fonction d'initialisation
    if (!init_brute_force_state(state, graph, instance_name)) {
        free_brute_force_state(state);
        return NULL;
    }

    return state;
}

/* --- Fonctions cœur de l'algorithme --- */

/**
 * @brief Évalue la permutation courante stockée dans 'state->perm'.
 * Met à jour les meilleurs/pires tours et longueurs.
 */
static void evaluate_current_permutation(BruteForceState *state) {
    // Construire le tour complet [0, perm..., 0]
    state->tour[0] = 0;
    for (int i = 0; i < state->size; i++) {
        state->tour[i + 1] = state->perm[i];
    }
    state->tour[state->n] = 0;

    // Calculer le coût
    double len = compute_tour_cost(state->graph, state->tour, state->n+1);
    state->iterations++;

    // Mettre à jour le meilleur
    if (len < state->best_len) {
        state->best_len = len;
        memcpy(state->best, state->perm, state->size * sizeof(int));
    }
    // Mettre à jour le pire
    if (len > state->worst_len) {
        state->worst_len = len;
        memcpy(state->worst, state->perm, state->size * sizeof(int));
    }
}

/**
 * @brief Gère l'interruption Ctrl+C.
 * Affiche l'état actuel et demande à l'utilisateur s'il veut arrêter.
 * @return 1 si l'utilisateur veut arrêter, 0 pour continuer.
 */
static int handle_interrupt(BruteForceState *state) {
    stop_flag = 0; // Réinitialiser le drapeau
    double elapsed = (double)(clock() - state->start_time) / CLOCKS_PER_SEC;

    printf("\n--- Interruption (Ctrl+C) ---\n");

    // Permutation courante (0-basée)
    printf("Permutation courante (0-based) : [");
    for (int i = 0; i < state->size; i++) {
        if (i) printf(", ");
        printf("%d", state->perm[i]);
    }
    printf("]\n");

    // Tournée courante (1-basée)
    printf("Tournée courante (1-based) : ");
    print_tour(state->perm, state->size);
    printf("\n");

    // Meilleur trouvé
    printf("Meilleure longueur : %.8f | Meilleure tournée : ", state->best_len);
    print_tour(state->best, state->size);
    printf("\n");

    // Pire trouvé
    printf("Pire longueur : %.0f | Pire tournée : ", state->worst_len);
    print_tour(state->worst, state->size);
    printf("\n");

    printf("Itérations : %llu | Temps CPU : %.3fs\n", state->iterations, elapsed);
    
    // Demander à l'utilisateur
    printf("Voulez-vous quitter ? [y/n] ");
    int c = getchar();
    while (c == '\n') c = getchar(); // Ignorer les sauts de ligne précédents

    int stop = (c == 'y' || c == 'Y');

    // Vider le buffer d'entrée
    while (c != '\n' && c != EOF) c = getchar(); 

    if (stop) {
        printf("Sortie demandée.\n");
        return 1; // Arrêter
    } else {
        signal(SIGINT, sigint_handler); // Ré-enregistrer le gestionnaire
        printf("Reprise du calcul...\n");
        return 0; // Continuer
    }
}


/* --- Fonction publique (Point d'entrée) --- */

/**
 * @brief Exécute l'algorithme de force brute pour le TSP.
 * (Implémentation de la fonction déclarée dans brute_force.h)
 */
void run_brute_force_graph(TSPGraph *graph, const char *instance_name) {
    if (!graph) return;
    if (graph->num_nodes <= 1) return;

    // Enregistrer le gestionnaire de signal
    signal(SIGINT, sigint_handler);

    // Initialiser l'état de l'algorithme
    BruteForceState *state = create_brute_force_state(graph, instance_name);
    if (!state) {
        exit(1); // Échec de l'allocation
    }

    int finished = 0;
    while (!finished) {
        // 1. Évaluer la permutation actuelle
        evaluate_current_permutation(state);

        // 2. Gérer une éventuelle interruption
        if (stop_flag) {
            if (handle_interrupt(state)) {
                break; // L'utilisateur a demandé l'arrêt
            }
        }

        // 3. Calculer la permutation suivante
        if (!next_permutation(state->perm, state->size)) {
            finished = 1; // C'était la dernière permutation
        }
    }

    if (finished) {
        print_final_results(state);
    }

    free_brute_force_state(state);
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/nearest_neighbor.h"
#include "../include/tour.h"
#include "../include/utils.h"
#include "../include/graph.h"

/* ================================================================
 *  Algorithme du plus proche voisin (Nearest Neighbor)
 * ================================================================ */

/**
 * @brief Trouve le voisin non visité le plus proche d'une ville donnée.
 * 
 * @param graph  Pointeur vers le graphe du TSP.
 * @param ville_actuelle  Indice de la ville courante.
 * @param villes_visitees Tableau binaire indiquant quelles villes sont déjà visitées.
 * @return L’indice du voisin le plus proche non visité, ou -1 si toutes sont visitées.
 */
static int trouver_voisin_plus_proche(TSPGraph *graph, int ville_actuelle, const int *villes_visitees)
{
    int n = graph->num_nodes;       // nombre total de villes
    double distance_min = 1e300;    // distance minimale trouvée (valeur très grande au début)
    int ville_plus_proche = -1;     // l'indice de la ville la plus proche (aucune au début)

    for (int ville = 1; ville < n; ++ville)
    {
        // Si la ville a déjà été visitée, on passe à la suivante
        if (villes_visitees[ville])
            continue;
        // On récupère la distance entre la ville actuelle et cette ville
        double distance = get_graph_distance(graph, ville_actuelle, ville);
        // Si cette distance est plus petite que la plus petite trouvée jusque-là,
        // on met à jour notre "meilleure" ville
        if (distance < distance_min)
        {
            distance_min = distance;
            ville_plus_proche = ville;
        }
    }
    return ville_plus_proche;
}


/**
 * @brief Construit la permutation des villes à visiter avec la méthode du plus proche voisin.
 * 
 * @param graph Graphe du TSP.
 * @param permutation_sortie Tableau où la permutation sera écrite (taille n-1).
 */
static void construire_permutation_nn(TSPGraph *graph, int *permutation_sortie) {
    int n = graph->num_nodes;
    int *villes_visitees = calloc(n, sizeof(int));
    if (!villes_visitees) {
        perror("Erreur d'allocation pour villes_visitees");
        exit(EXIT_FAILURE);
    }

    villes_visitees[0] = 1; // on commence toujours à la ville 0
    int ville_actuelle = 0;

        for (int i = 0; i < n - 1; ++i)
        {
        // Trouver la ville non visitée la plus proche
        int prochaine_ville = trouver_voisin_plus_proche(graph, ville_actuelle, villes_visitees);
        // On l’ajoute à la permutation
        permutation_sortie[i] = prochaine_ville;
        // On la marque comme visitée
        villes_visitees[prochaine_ville] = 1;
        // Et elle devient la nouvelle ville actuelle
        ville_actuelle = prochaine_ville;
    }
    free(villes_visitees);
}

/**
 * @brief Libère proprement les tableaux utilisés par l'algorithme NN.
 *
 * @param permutation   Tableau contenant la permutation.
 * @param tour_complet  Tableau contenant la tournée complète.
 */
static void liberer_tableaux_nn(int *permutation, int *tour_complet) {
    if (permutation)
        free(permutation);
    if (tour_complet)
        free(tour_complet);
}

/**
 * @brief Exécute l'algorithme du plus proche voisin et affiche les résultats.
 * 
 * @param graph Graphe du TSP.
 * @param instance_name Nom de l’instance (pour affichage).
 */
void run_nearest_neighbor(TSPGraph *graph, const char *instance_name) {
    if (!graph || graph->num_nodes <= 1) return;

    int n = graph->num_nodes;
    int *permutation = malloc((n - 1) * sizeof(int));
    int *tour_complet = malloc((n + 1) * sizeof(int));

    if (!permutation || !tour_complet) {
        perror("Erreur d'allocation pour nearest neighbor");
        liberer_tableaux_nn(permutation, tour_complet);
        return;
    }

    clock_t debut = clock();

    // Étape 1 : construire la permutation
    construire_permutation_nn(graph, permutation);

    // Étape 2 : créer la tournée complète [0, perm..., 0]
    tour_complet[0] = 0;
    for (int i = 0; i < n - 1; ++i)
        tour_complet[i + 1] = permutation[i];
    tour_complet[n] = 0;

    // Étape 3 : calculer la longueur et le temps d’exécution
    double longueur = compute_tour_cost(graph, tour_complet, n + 1);
    double temps_cpu = (double)(clock() - debut) / CLOCKS_PER_SEC;

    // Étape 4 : affichage standardisé
    printf("Instance ; Methode ; Temps CPU (sec) ; Longueur ; Tour\n");
    printf("%s ; nn ; %.3f ; %.12f ; ", instance_name, temps_cpu, longueur);
    print_tour(permutation, n - 1);  // Tour
    printf("\n");

    liberer_tableaux_nn(permutation, tour_complet);
}

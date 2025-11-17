#ifndef TWO_OPT_H
#define TWO_OPT_H

#include "graph.h"
#include "tsp_io.h"

/**
 * @brief Applique 2-opt à une tournée existante.
 * 
 * @param graph Graphe TSP (pour accéder aux distances)
 * @param tour Tournée (tableau d'indices)
 * @param n Nombre de villes
 * @return double Nouvelle longueur après optimisation
 */
double two_opt_optimize(TSPGraph *graph, int *tour, int n);

/**
 * @brief Fait un Random Walk puis l'améliore avec 2-opt.
 */
void run_two_opt_random_walk(TSPGraph *graph, const char *instance_name);

/**
 * @brief Fait un Nearest Neighbor puis l'améliore avec 2-opt.
 */
void run_two_opt_nearest_neighbor(TSPGraph *graph, const char *instance_name);

#endif

#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include "graph.h"


/* ================================================================
 *  Interface du module "Nearest Neighbor"
 * ================================================================ */

/**
 * @brief Exécute l'algorithme du plus proche voisin et affiche les résultats.
 * 
 * @param graph Graphe du TSP.
 * @param instance_name Nom de l’instance (pour affichage).
 */
void run_nearest_neighbor(TSPGraph *graph, const char *instance_name);

/**
 * @brief Exécute l'algorithme du plus proche voisin pour 2-opt.
 * 
 * @param graph Graphe du TSP.
 * @param instance_name Nom de l’instance (pour affichage).
 */
int* calculate_2opt(TSPGraph *graph, const char *instance_name);


#endif

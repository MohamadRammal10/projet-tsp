#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include "graph.h"

/**
 * @brief Exécute l'algorithme de la marche aléatoire (Random Walk) sur une instance TSP.
 * 
 * @param graph Graphe du TSP (structure créée à partir de l'instance).
 * @param instance_name Nom de l’instance (pour affichage normalisé).
 */
void run_random_walk(TSPGraph *graph, const char *instance_name);

#endif

#ifndef GENETIC_H
#define GENETIC_H

#include "graph.h"

/**
 * @brief Lance l'algorithme génétique sur le graphe TSP et affiche le résultat.
 *
 * Implémente une version C de tsp-tri-light.py :
 * - sélection par tournoi
 * - croisement "ordered crossover"
 * - mutation par swap
 * - remplacement du pire par un individu aléatoire
 * - élitisme : le meilleur global est réinjecté à chaque génération
 *
 * @param graph          Graphe TSP (distances déjà construites).
 * @param instance_name  Nom de l’instance (pour affichage).
 */
void run_genetic_algorithm(TSPGraph *graph, const char *instance_name);

#endif /* GENETIC_H */
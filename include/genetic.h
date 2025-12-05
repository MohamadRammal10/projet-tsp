#ifndef GENETIC_H
#define GENETIC_H

#include "graph.h"

/**
 * @brief Lance l'algorithme génétique standard (croisement OX) sur le graphe TSP.
 *
 * Implémente une version C de tsp-tri-light.py :
 * - sélection par tournoi
 * - croisement "ordered crossover"
 * - mutation par swap
 * - remplacement du pire par un individu aléatoire
 * - élitisme : le meilleur global est réinjecté à chaque génération
 */
void run_genetic_algorithm(TSPGraph *graph,
                           const char *instance_name,
                           int pop_size,
                           int generations,
                           double mutation_rate);

/**
 * @brief Variante GA + DPX + 2-opt (gadpx).
 *
 * Identique au GA standard, mais le croisement est remplacé par le
 * DPX (Distance Preserving Crossover) spécifique au TSP, puis chaque
 * enfant est amélioré par 2-opt.
 */
void run_genetic_algorithm_dpx(TSPGraph *graph,
                               const char *instance_name,
                               int pop_size,
                               int generations,
                               double mutation_rate);

#endif /* GENETIC_H */

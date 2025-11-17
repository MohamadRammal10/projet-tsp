#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "../include/genetic.h"
#include "../include/tour.h"
#include "../include/graph.h"

// =======================
// Paramètres du GA
// =======================
#define GA_POPULATION_SIZE     30
#define GA_GENERATIONS         1000
#define GA_MUTATION_RATE       0.10
#define GA_TOURNAMENT_RATIO    0.5   // 50% de la population

typedef struct {
    int   *perm;     // permutation de 0..n-1
    double fitness;  // longueur de la tournée
} GAIndividual;

typedef struct {
    GAIndividual *population;
    GAIndividual *offspring;
    int          *selected_indices;
    int          *best_perm;
    double        best_fitness;

    int pop_size;
    int n;
    int generations;
    int tournament_size;
    double mutation_rate;

} GAContext;

// -----------------------
// Fonctions utilitaires bas niveau
// -----------------------

static double ga_tour_cost(TSPGraph *graph, const int *perm, int n) {
    double total = 0.0;
    for (int i = 0; i < n - 1; ++i) {
        total += get_graph_distance(graph, perm[i], perm[i + 1]);
    }
    // retour au départ
    total += get_graph_distance(graph, perm[n - 1], perm[0]);
    return total;
}

static void ga_random_permutation(int *perm, int n) {
    for (int i = 0; i < n; ++i) {
        perm[i] = i;
    }
    // Fisher-Yates
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
}

static void ga_init_population(GAIndividual *pop, int pop_size, int n, TSPGraph *graph) {
    for (int i = 0; i < pop_size; ++i) {
        pop[i].perm = (int *)malloc(n * sizeof(int));
        if (!pop[i].perm) {
            fprintf(stderr, "Malloc error in ga_init_population\n");
            exit(EXIT_FAILURE);
        }
        ga_random_permutation(pop[i].perm, n);
        pop[i].fitness = ga_tour_cost(graph, pop[i].perm, n);
    }
}

static void ga_free_population(GAIndividual *pop, int pop_size) {
    if (!pop) return;
    for (int i = 0; i < pop_size; ++i) {
        free(pop[i].perm);
    }
}

static void ga_tournament_selection(
    GAIndividual *population,
    int pop_size,
    int tournament_size,
    int *selected_indices
) {
    for (int i = 0; i < pop_size; ++i) {
        int best_idx = -1;
        double best_fit = DBL_MAX;

        for (int t = 0; t < tournament_size; ++t) {
            int idx = rand() % pop_size;
            double f = population[idx].fitness;
            if (f < best_fit) {
                best_fit = f;
                best_idx = idx;
            }
        }
        selected_indices[i] = best_idx;
    }
}

// Ordered Crossover (OX)
static void ga_ordered_crossover(
    const int *parent_a,
    const int *parent_b,
    int *child,
    int n
) {
    int start = rand() % n;
    int end   = rand() % n;
    if (start > end) {
        int tmp = start;
        start = end;
        end = tmp;
    }

    // init enfant
    for (int i = 0; i < n; ++i) {
        child[i] = -1;
    }

    // segment de A
    for (int i = start; i < end; ++i) {
        child[i] = parent_a[i];
    }

    // compléter avec B
    int pos = 0;
    for (int i = 0; i < n; ++i) {
        int gene = parent_b[i];
        int present = 0;

        for (int j = 0; j < n; ++j) {
            if (child[j] == gene) {
                present = 1;
                break;
            }
        }
        if (!present) {
            while (child[pos] != -1) {
                ++pos;
            }
            child[pos] = gene;
        }
    }
}

// Mutation par swap
static void ga_swap_mutation(int *perm, int n, double mutation_rate) {
    for (int i = 0; i < n; ++i) {
        double r = (double)rand() / (double)RAND_MAX;
        if (r < mutation_rate) {
            int j = rand() % n;
            int tmp = perm[i];
            perm[i] = perm[j];
            perm[j] = tmp;
        }
    }
}

static int ga_compare_individuals(const void *a, const void *b) {
    double fa = ((const GAIndividual *)a)->fitness;
    double fb = ((const GAIndividual *)b)->fitness;
    if (fa < fb) return -1;
    if (fa > fb) return  1;
    return 0;
}

// -----------------------
// Gestion du contexte GA
// -----------------------

static void ga_init_context(GAContext *ctx, TSPGraph *graph) {
    ctx->pop_size      = GA_POPULATION_SIZE;
    ctx->generations   = GA_GENERATIONS;
    ctx->mutation_rate = GA_MUTATION_RATE;
    ctx->n             = graph->num_nodes;

    ctx->tournament_size = (int)(GA_TOURNAMENT_RATIO * ctx->pop_size);
    if (ctx->tournament_size < 2) ctx->tournament_size = 2;
    if (ctx->tournament_size > ctx->pop_size) ctx->tournament_size = ctx->pop_size;

    ctx->population = (GAIndividual *)malloc(ctx->pop_size * sizeof(GAIndividual));
    ctx->offspring  = (GAIndividual *)malloc(ctx->pop_size * sizeof(GAIndividual));
    if (!ctx->population || !ctx->offspring) {
        fprintf(stderr, "Malloc error in ga_init_context (population)\n");
        exit(EXIT_FAILURE);
    }

    // perm des enfants
    for (int i = 0; i < ctx->pop_size; ++i) {
        ctx->offspring[i].perm = (int *)malloc(ctx->n * sizeof(int));
        if (!ctx->offspring[i].perm) {
            fprintf(stderr, "Malloc error in ga_init_context (offspring perm)\n");
            exit(EXIT_FAILURE);
        }
    }

    ctx->selected_indices = (int *)malloc(ctx->pop_size * sizeof(int));
    if (!ctx->selected_indices) {
        fprintf(stderr, "Malloc error for selected_indices\n");
        exit(EXIT_FAILURE);
    }

    ctx->best_perm = (int *)malloc(ctx->n * sizeof(int));
    if (!ctx->best_perm) {
        fprintf(stderr, "Malloc error for best_perm\n");
        exit(EXIT_FAILURE);
    }

    ctx->best_fitness = DBL_MAX;
}

static void ga_free_context(GAContext *ctx) {
    if (!ctx) return;
    ga_free_population(ctx->population, ctx->pop_size);
    ga_free_population(ctx->offspring, ctx->pop_size);
    free(ctx->selected_indices);
    free(ctx->best_perm);
}

// -----------------------
// Étapes "haut niveau" du GA
// -----------------------

static void ga_initialize_population_and_best(GAContext *ctx, TSPGraph *graph) {
    ga_init_population(ctx->population, ctx->pop_size, ctx->n, graph);

    ctx->best_fitness = DBL_MAX;
    for (int i = 0; i < ctx->pop_size; ++i) {
        if (ctx->population[i].fitness < ctx->best_fitness) {
            ctx->best_fitness = ctx->population[i].fitness;
            for (int k = 0; k < ctx->n; ++k) {
                ctx->best_perm[k] = ctx->population[i].perm[k];
            }
        }
    }
}

// 1 génération = sélection + croisement + mutation + tri + remplacement
static void ga_create_offspring(GAContext *ctx, TSPGraph *graph) {
    // 1) sélection par tournoi
    ga_tournament_selection(
        ctx->population,
        ctx->pop_size,
        ctx->tournament_size,
        ctx->selected_indices
    );

    // 2) croisement
    for (int i = 0; i < ctx->pop_size; i += 2) {
        int idx_a = ctx->selected_indices[i];
        int idx_b = ctx->selected_indices[(i + 1) % ctx->pop_size];

        GAIndividual *pa = &ctx->population[idx_a];
        GAIndividual *pb = &ctx->population[idx_b];

        ga_ordered_crossover(pa->perm, pb->perm, ctx->offspring[i].perm, ctx->n);
        if (i + 1 < ctx->pop_size) {
            ga_ordered_crossover(pb->perm, pa->perm, ctx->offspring[i + 1].perm, ctx->n);
        }
    }

    // 3) mutation + évaluation
    for (int i = 0; i < ctx->pop_size; ++i) {
        ga_swap_mutation(ctx->offspring[i].perm, ctx->n, ctx->mutation_rate);
        ctx->offspring[i].fitness = ga_tour_cost(graph, ctx->offspring[i].perm, ctx->n);
    }

    // 4) tri des enfants par fitness
    qsort(ctx->offspring, ctx->pop_size, sizeof(GAIndividual), ga_compare_individuals);

    // 5) population <- enfants (swap de pointeurs)
    GAIndividual *tmp = ctx->population;
    ctx->population = ctx->offspring;
    ctx->offspring  = tmp;
}

// remplace le pire individu par un individu aléatoire
static void ga_inject_random_individual(GAContext *ctx, TSPGraph *graph) {
    int worst_idx = 0;
    double worst_fit = ctx->population[0].fitness;
    for (int i = 1; i < ctx->pop_size; ++i) {
        if (ctx->population[i].fitness > worst_fit) {
            worst_fit = ctx->population[i].fitness;
            worst_idx = i;
        }
    }

    ga_random_permutation(ctx->population[worst_idx].perm, ctx->n);
    ctx->population[worst_idx].fitness =
        ga_tour_cost(graph, ctx->population[worst_idx].perm, ctx->n);
}

// met à jour le meilleur global et applique l'élitisme fort
static void ga_update_best_and_elitism(GAContext *ctx) {
    // meilleur de la génération
    int best_gen_idx = 0;
    double best_gen_fit = ctx->population[0].fitness;
    for (int i = 1; i < ctx->pop_size; ++i) {
        if (ctx->population[i].fitness < best_gen_fit) {
            best_gen_fit = ctx->population[i].fitness;
            best_gen_idx = i;
        }
    }

    // mise à jour best global
    if (best_gen_fit < ctx->best_fitness) {
        ctx->best_fitness = best_gen_fit;
        for (int k = 0; k < ctx->n; ++k) {
            ctx->best_perm[k] = ctx->population[best_gen_idx].perm[k];
        }
    }

    // on retrouve le pire individu pour y injecter le best global
    int worst_idx = 0;
    double worst_fit = ctx->population[0].fitness;
    for (int i = 1; i < ctx->pop_size; ++i) {
        if (ctx->population[i].fitness > worst_fit) {
            worst_fit = ctx->population[i].fitness;
            worst_idx = i;
        }
    }

    for (int k = 0; k < ctx->n; ++k) {
        ctx->population[worst_idx].perm[k] = ctx->best_perm[k];
    }
    ctx->population[worst_idx].fitness = ctx->best_fitness;
}

// boucle principale sur les générations
static void ga_run_generations(GAContext *ctx, TSPGraph *graph) {
    for (int gen = 0; gen < ctx->generations; ++gen) {
        ga_create_offspring(ctx, graph);
        ga_inject_random_individual(ctx, graph);
        ga_update_best_and_elitism(ctx);
    }
}

// -----------------------
// Fonction principale = pseudo-code propre
// -----------------------

void run_genetic_algorithm(TSPGraph *graph, const char *instance_name) {
    if (!graph) {
        fprintf(stderr, "run_genetic_algorithm: graph is NULL\n");
        return;
    }
    if (graph->num_nodes <= 1) {
        fprintf(stderr, "run_genetic_algorithm: not enough nodes\n");
        return;
    }

    // 0) init RNG
    srand((unsigned int)time(NULL));

    // 1) créer le contexte
    GAContext ctx;
    ga_init_context(&ctx, graph);

    // 2) initialiser la population + meilleur global
    ga_initialize_population_and_best(&ctx, graph);

    // 3) faire évoluer la population
    clock_t start = clock();
    ga_run_generations(&ctx, graph);
    double secs = (double)(clock() - start) / CLOCKS_PER_SEC;

    // 4) afficher le résultat final
    printf("Instance ; Methode ; Temps CPU (sec) ; Longueur ; Tour\n");
    printf("%s ; ga ; %.3f ; %.12f ; ", instance_name, secs, ctx.best_fitness);

    printf("[");
    printf("%d", ctx.best_perm[0] + 1);
    for (int i = 1; i < ctx.n; ++i) {
        printf(", %d", ctx.best_perm[i] + 1);
    }
    printf("]\n");

    // 5) libérer le contexte
    ga_free_context(&ctx);
}
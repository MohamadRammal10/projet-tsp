#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "../include/genetic.h"
#include "../include/tour.h"
#include "../include/graph.h"
#include "../include/two_opt.h"

// =======================
// Paramètres du GA
// =======================
#define GA_TOURNAMENT_RATIO 0.5   // 50% de la population

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
    free(pop);
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

static void ga_init_context(GAContext *ctx, TSPGraph *graph, int pop_size, int generations, double mutation_rate) {
    ctx->pop_size      = pop_size;
    ctx->generations   = generations;
    ctx->mutation_rate = mutation_rate;
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
// Étapes "haut niveau" du GA (version OX standard)
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

    // 2) croisement (OX)
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
    ctx->population   = ctx->offspring;
    ctx->offspring    = tmp;
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

// boucle principale sur les générations (version OX)
static void ga_run_generations(GAContext *ctx, TSPGraph *graph) {
    for (int gen = 0; gen < ctx->generations; ++gen) {
        ga_create_offspring(ctx, graph);
        ga_inject_random_individual(ctx, graph);
        ga_update_best_and_elitism(ctx);
    }
}

// -----------------------
// Partie DPX
// -----------------------

typedef struct {
    int start;      // index de début dans parent1
    int length;     // nombre de villes dans le fragment
} DPXFragment;

// construit les fragments : parent1 est découpé selon les arêtes communes avec parent2
static int dpx_build_fragments(const int *parent1, const int *parent2, int n,
                               DPXFragment *frags) {
    int *pos2 = (int *)malloc(n * sizeof(int));
    if (!pos2) {
        fprintf(stderr, "Malloc error in dpx_build_fragments pos2\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) {
        pos2[parent2[i]] = i;
    }

    int frag_start = 0;
    int nb_frag = 0;

    for (int i = 0; i < n - 1; ++i) {
        int a = parent1[i];
        int b = parent1[i + 1];

        int idx = pos2[a];
        int left  = parent2[(idx - 1 + n) % n];
        int right = parent2[(idx + 1) % n];

        int common = (b == left || b == right);

        if (!common) {
            frags[nb_frag].start  = frag_start;
            frags[nb_frag].length = i - frag_start + 1;
            nb_frag++;
            frag_start = i + 1;
        }
    }

    // dernier fragment
    frags[nb_frag].start  = frag_start;
    frags[nb_frag].length = n - frag_start;
    nb_frag++;

    free(pos2);
    return nb_frag;
}

static int edge_in_parent(const int *perm, const int *pos, int n, int u, int v) {
    int i = pos[u];
    int left  = perm[(i - 1 + n) % n];
    int right = perm[(i + 1) % n];
    return (v == left || v == right);
}

static int edge_in_parents(const int *p1, const int *pos1,
                           const int *p2, const int *pos2,
                           int n, int u, int v) {
    return edge_in_parent(p1, pos1, n, u, v) ||
           edge_in_parent(p2, pos2, n, u, v);
}

// ---- Helpers factorisés pour la reconnexion DPX ----

static void dpx_build_pos_arrays(const int *parent1, const int *parent2,
                                 int n, int *pos1, int *pos2) {
    for (int i = 0; i < n; ++i) {
        pos1[parent1[i]] = i;
        pos2[parent2[i]] = i;
    }
}

static int dpx_choose_start_fragment(const DPXFragment *frags, int nb_frag) {
    int start_frag = 0;
    int best_len   = frags[0].length;
    for (int f = 1; f < nb_frag; ++f) {
        if (frags[f].length > best_len) {
            best_len   = frags[f].length;
            start_frag = f;
        }
    }
    return start_frag;
}

static void dpx_append_fragment_to_child(const int *parent1,
                                         const DPXFragment *frag,
                                         int orient,   // 0 = normal, 1 = inversé
                                         int *child,
                                         int *child_pos,
                                         int *current_end) {
    if (orient == 0) {
        // orientation normale
        for (int k = 0; k < frag->length; ++k) {
            child[*child_pos] = parent1[frag->start + k];
            (*child_pos)++;
        }
        *current_end = parent1[frag->start + frag->length - 1];
    } else {
        // orientation inversée
        for (int k = frag->length - 1; k >= 0; --k) {
            child[*child_pos] = parent1[frag->start + k];
            (*child_pos)++;
        }
        *current_end = parent1[frag->start]; 
    }
}

// choisit le meilleur fragment à connecter suivant la logique DPX
static void dpx_find_best_fragment_to_connect(const int *parent1,
                                              const int *parent2,
                                              int n,
                                              const int *pos1,
                                              const int *pos2,
                                              const DPXFragment *frags,
                                              int nb_frag,
                                              const int *used,
                                              int current_end,
                                              TSPGraph *graph,
                                              int *chosen_frag,
                                              int *chosen_orient) {
    double best_allowed_dist = DBL_MAX;
    int best_allowed_frag    = -1;
    int best_allowed_orient  = 0;

    double best_any_dist = DBL_MAX;
    int best_any_frag    = -1;
    int best_any_orient  = 0;

    for (int f = 0; f < nb_frag; ++f) {
        if (used[f]) continue;

        const DPXFragment *frag = &frags[f];
        int start_city = parent1[frag->start];
        int end_city   = parent1[frag->start + frag->length - 1];

        // candidat: connecter current_end -> start_city (normal)
        {
            double d = get_graph_distance(graph, current_end, start_city);
            int forbidden = edge_in_parents(parent1, pos1, parent2, pos2,
                                            n, current_end, start_city);

            if (!forbidden && d < best_allowed_dist) {
                best_allowed_dist = d;
                best_allowed_frag = f;
                best_allowed_orient = 0;
            }
            if (d < best_any_dist) {
                best_any_dist = d;
                best_any_frag = f;
                best_any_orient = 0;
            }
        }

        // candidat: connecter current_end -> end_city (inversé)
        {
            double d = get_graph_distance(graph, current_end, end_city);
            int forbidden = edge_in_parents(parent1, pos1, parent2, pos2,
                                            n, current_end, end_city);

            if (!forbidden && d < best_allowed_dist) {
                best_allowed_dist = d;
                best_allowed_frag = f;
                best_allowed_orient = 1;
            }
            if (d < best_any_dist) {
                best_any_dist = d;
                best_any_frag = f;
                best_any_orient = 1;
            }
        }
    }

    if (best_allowed_frag != -1) {
        *chosen_frag   = best_allowed_frag;
        *chosen_orient = best_allowed_orient;
    } else {
        *chosen_frag   = best_any_frag;
        *chosen_orient = best_any_orient;
    }
}

// reconnecter les fragments (version factorisée)
static void dpx_reconnect_fragments(const int *parent1, const int *parent2,
                                    int n,
                                    const DPXFragment *frags, int nb_frag,
                                    int *child, TSPGraph *graph) {
    int *pos1 = (int *)malloc(n * sizeof(int));
    int *pos2 = (int *)malloc(n * sizeof(int));
    int *used = (int *)calloc(nb_frag, sizeof(int));
    if (!pos1 || !pos2 || !used) {
        fprintf(stderr, "Malloc error in dpx_reconnect_fragments\n");
        exit(EXIT_FAILURE);
    }

    dpx_build_pos_arrays(parent1, parent2, n, pos1, pos2);

    int start_frag = dpx_choose_start_fragment(frags, nb_frag);

    int child_pos = 0;
    int current_end;

    // placer le fragment de départ
    dpx_append_fragment_to_child(parent1, &frags[start_frag],
                                 0, child, &child_pos, &current_end);
    used[start_frag] = 1;
    int nb_used = 1;

    // tant qu'il reste des fragments à connecter
    while (nb_used < nb_frag) {
        int chosen_frag;
        int chosen_orient;
        dpx_find_best_fragment_to_connect(parent1, parent2, n,
                                          pos1, pos2,
                                          frags, nb_frag,
                                          used,
                                          current_end,
                                          graph,
                                          &chosen_frag,
                                          &chosen_orient);

        if (chosen_frag < 0) {
            // cas pathologique : on ne trouve rien, on casse la boucle
            break;
        }

        dpx_append_fragment_to_child(parent1, &frags[chosen_frag],
                                     chosen_orient,
                                     child, &child_pos, &current_end);
        used[chosen_frag] = 1;
        nb_used++;
    }

    free(pos1);
    free(pos2);
    free(used);
}

// croisement DPX complet, factorisé
static void ga_dpx_crossover(
    const int *parent1,
    const int *parent2,
    int *child,
    int n,
    TSPGraph *graph
) {
    DPXFragment *frags = (DPXFragment *)malloc(n * sizeof(DPXFragment));
    if (!frags) {
        fprintf(stderr, "Malloc error in ga_dpx_crossover (frags)\n");
        exit(EXIT_FAILURE);
    }

    int nb_frag = dpx_build_fragments(parent1, parent2, n, frags);
    dpx_reconnect_fragments(parent1, parent2, n, frags, nb_frag, child, graph);

    free(frags);

    // amélioration locale avec 2-opt sur l'enfant
    (void)two_opt_optimize(graph, child, n);
}

// 1 génération GA avec croisement DPX
static void ga_create_offspring_dpx(GAContext *ctx, TSPGraph *graph) {
    // 1) sélection par tournoi
    ga_tournament_selection(
        ctx->population,
        ctx->pop_size,
        ctx->tournament_size,
        ctx->selected_indices
    );

    // 2) croisement DPX
    for (int i = 0; i < ctx->pop_size; i += 2) {
        int idx_a = ctx->selected_indices[i];
        int idx_b = ctx->selected_indices[(i + 1) % ctx->pop_size];

        GAIndividual *pa = &ctx->population[idx_a];
        GAIndividual *pb = &ctx->population[idx_b];

        ga_dpx_crossover(pa->perm, pb->perm, ctx->offspring[i].perm, ctx->n, graph);
        if (i + 1 < ctx->pop_size) {
            ga_dpx_crossover(pb->perm, pa->perm, ctx->offspring[i + 1].perm, ctx->n, graph);
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
    ctx->population   = ctx->offspring;
    ctx->offspring    = tmp;
}

// boucle principale GA avec DPX
static void ga_run_generations_dpx(GAContext *ctx, TSPGraph *graph) {
    for (int gen = 0; gen < ctx->generations; ++gen) {
        ga_create_offspring_dpx(ctx, graph);
        ga_inject_random_individual(ctx, graph);
        ga_update_best_and_elitism(ctx);
    }
}

// -----------------------
// Fonctions principales
// -----------------------

void run_genetic_algorithm(TSPGraph *graph, const char *instance_name, int pop_size, int generations, double mutation_rate) {
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
    ga_init_context(&ctx, graph, pop_size, generations, mutation_rate);

    // 2) initialiser la population + meilleur global
    ga_initialize_population_and_best(&ctx, graph);

    // 3) faire évoluer la population (version OX standard)
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

void run_genetic_algorithm_dpx(TSPGraph *graph, const char *instance_name, int pop_size, int generations, double mutation_rate) {
    if (!graph) {
        fprintf(stderr, "run_genetic_algorithm_dpx: graph is NULL\n");
        return;
    }
    if (graph->num_nodes <= 1) {
        fprintf(stderr, "run_genetic_algorithm_dpx: not enough nodes\n");
        return;
    }

    srand((unsigned int)time(NULL));

    GAContext ctx;
    ga_init_context(&ctx, graph, pop_size, generations, mutation_rate);

    ga_initialize_population_and_best(&ctx, graph);

    clock_t start = clock();
    ga_run_generations_dpx(&ctx, graph);
    double secs = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("Instance ; Methode ; Temps CPU (sec) ; Longueur ; Tour\n");
    printf("%s ; gadpx ; %.3f ; %.12f ; ", instance_name, secs, ctx.best_fitness);

    printf("[");
    printf("%d", ctx.best_perm[0] + 1);
    for (int i = 1; i < ctx.n; ++i) {
        printf(", %d", ctx.best_perm[i] + 1);
    }
    printf("]\n");

    ga_free_context(&ctx);
}

(english/french)

# Traveling Salesman Problem (TSP)

[Project management](https://docs.google.com/spreadsheets/d/1D0tdRev2FNDUF0lIM8KSrpiOma9Gkf4VGkew63VwrPA/edit?gid=871870674#gid=871870674)

## Team Introduction

Our development team consists of four members :

* **Mohamad RAMMAL** – RP
* **Fourat AMMARI** – RSD
* **Mohamed Yassine BOUZAIENE** – REH
* **Oleksandra FILIUSHKINA** – RME

---

## Introduction

This project focuses on solving an optimization problem such as the Traveling Salesman Problem, aiming to implement and compare various algorithms while analyzing their performance. The work was carried out as part of an academic development project, in which the team collaborated to design, implement, test, and evaluate different solution methods. The project includes multiple algorithmic approaches, performance evaluations, visualization tools, and a structured testing environment.

---

## Problem Description

The core problem addressed in this project involves finding an efficient route in order to visit all cities on a given graph. The objective is to compare performance across multiple strategies.

The main challenges include:

* Understanding and implementing classical algorithms (e.g., brute force, nearest neighbor)
* Integrating probabilistic or randomized search techniques (e.g., random walk)
* Implementing optimization procedures (e.g., 2‑opt improvement heuristic)
* Exploring advanced metaheuristics (e.g., genetic algorithms)
* Ensuring fair comparison through consistent testing and benchmarking
* Building a maintainable and clear project structure


---


### Available Modes / Methods

The project supports several algorithmic modes:

| Method       | Description                                                                   |
| ------------ | ----------------------------------------------------------------------------- |
| `bruteforce` | Exhaustively explores all possible solutions. Guarantees optimality but slow. |
| `nearest`    | Greedy algorithm selecting the nearest available node at each step.           |
| `randomwalk` | Generates random valid tours, useful for baseline comparisons.                |
| `twoopt`     | Applies 2‑opt local improvement to an existing tour.                          |
| `genetic`    | Uses evolutionary strategies: selection, crossover, mutation.                 |

Example:

```
./tsp -f tests/example.tsp -c -m all
```

---


## Usage

![Image](https://github.com/h1tkx/projet-tsp/blob/main/structure.svg)
### Running the Main Program

  -> Make options :

  make              Compile project
  make debug        Compile project + debug mode
  make clean        Clean .o files
    
  To execute the main program, use:

  ```
  ./tsp -f <file> [-c] [-m <mode>]
  ```

  -> Options :

  ```
  -h           Show this help message.
  -f <file>    TSPLIB instance file
  -c           Run canonical search
  -m all       Run all algorithms
  -m bf        Run brute-force search
  -m nn        Run nearest-neighbor search
  -m rw        Run random walk search
  -m 2optnn    Run 2-opt nearest-neighbor search
  -m 2optrw    Run 2-opt random walk search
  -m ga <pop_size> <num_generations> <mutation_rate>       Test genetic algorithm mode
  -m gadpx <pop_size> <num_generations> <mutation_rate>    Genetic algorithm (DPX + 2-opt)
  ```

---


### Additional Options for the Genetic Algorithm

* `pop_size` : Choose the size of the population. (default 100)
* `num_generations` : Choose the number of generations. (default 1000)
* `mutation_rate` : Set the mutation rate. (default 0.02)
---

## Running the Tests

Tests are located in the `tests/` directory. Run them with:

```
python3 test_tsp_c.py <mode>
```

Mode options :

```
-c          Test canonical mode
bf          Test brute force mode
nn          Test nearest neighbor mode
rw          Test random walk mode
2optnn      Test 2-opt nearest-neighbor mode
2optrw      Test 2-opt random walk mode
ga          Test genetic algorithm mode
gadpx       Test genetic algorithm mode (DPX + 2-opt)
```

Tests are validated if they get the same tour result compared to TSPLIB95 library.


# Problème du Voyageur de Commerce (TSP)

[Gestion de projet](https://docs.google.com/spreadsheets/d/1D0tdRev2FNDUF0lIM8KSrpiOma9Gkf4VGkew63VwrPA/edit?gid=871870674#gid=871870674)

## Présentation de l’équipe

Notre équipe de développement est composée de quatre membres :

* **Mohamad RAMMAL** – RP
* **Fourat AMMARI** – RSD
* **Mohamed Yassine BOUZAIENE** – REH
* **Oleksandra FILIUSHKINA** – RME

---

## Introduction

Ce projet porte sur la résolution d’un problème d’optimisation, notamment le Problème du Voyageur de Commerce (TSP). Il vise à implémenter et comparer plusieurs algorithmes tout en analysant leurs performances. Le travail a été réalisé dans le cadre d’un projet académique, où l’équipe a collaboré pour concevoir, implémenter, tester et évaluer différentes méthodes de résolution. Le projet inclut des approches algorithmiques variées, des évaluations de performance, des outils de visualisation et un environnement structuré pour les tests.

---

## Description du problème

Le problème principal étudié dans ce projet consiste à trouver un chemin efficace permettant de visiter toutes les villes d’un graphe donné. L’objectif est de comparer les performances de différentes stratégies.

Les principaux défis incluent :

* Comprendre et implémenter des algorithmes classiques (ex : brute force, plus proche voisin)
* Intégrer des techniques de recherche probabilistes ou aléatoires (ex : random walk)
* Implémenter des procédures d’optimisation (ex : heuristique d’amélioration 2‑opt)
* Explorer des métaheuristiques avancées (ex : algorithme génétique)
* Garantir une comparaison équitable via des tests et benchmarks cohérents
* Construire une structure de projet claire et maintenable

---

### Modes / Méthodes disponibles

Le projet prend en charge plusieurs modes algorithmiques :

| Méthode      | Description                                                                  |
| ------------ | ---------------------------------------------------------------------------- |
| `bruteforce` | Explore exhaustivement toutes les solutions. Optimal mais très lent.         |
| `nearest`    | Algorithme glouton choisissant à chaque étape le nœud le plus proche.        |
| `randomwalk` | Génère des tournées aléatoires valides, utile pour des comparaisons simples. |
| `twoopt`     | Applique une amélioration locale 2‑opt à une tournée existante.              |
| `genetic`    | Utilise des stratégies évolutives : sélection, croisement, mutation.         |

Exemple :

```
./tsp -f tests/example.tsp -c -m all
```

---

## Utilisation

![Image](https://github.com/h1tkx/projet-tsp/blob/main/structure.svg)

### Exécution du programme principal

-> Commandes Make :

```
make            Compile le projet
make debug      Compile le projet en mode debug
make clean      Supprime les fichiers .o
```

Pour exécuter le programme principal, utilisez :

```
./tsp -f <fichier> [-c] [-m <mode>]
```

-> Options :

```
-h           Affiche l’aide
-f <file>    Fichier d’instance TSPLIB
-c           Effectue une recherche canonique
-m all       Exécute tous les algorithmes
-m bf        Force brute
-m nn        Plus proche voisin
-m rw        Random walk
-m 2optnn    2-opt sur nearest-neighbor
-m 2optrw    2-opt sur random walk
-m ga <pop_size> <num_generations> <mutation_rate>       Algorithme génétique
-m gadpx <pop_size> <num_generations> <mutation_rate>    AG (DPX + 2-opt)
```

---

### Options supplémentaires pour l’algorithme génétique

* `pop_size` : Taille de la population (défaut : 100)
* `num_generations` : Nombre de générations (défaut : 1000)
* `mutation_rate` : Taux de mutation (défaut : 0.02)

---

## Exécution des tests

Les tests se trouvent dans le dossier `tests/`. Pour les exécuter :

```
python3 test_tsp_c.py <mode>
```

Modes disponibles :

```
-c          Teste le mode canonique
bf          Test brute force
nn          Test plus proche voisin
rw          Test random walk
2optnn      Test 2-opt nearest-neighbor
2optrw      Test 2-opt random walk
ga          Test algorithme génétique
gadpx       Test AG (DPX + 2-opt)
```

Les tests sont validés si la tournée obtenue correspond à celle générée par la bibliothèque TSPLIB95.

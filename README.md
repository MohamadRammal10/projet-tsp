# Traveling Salesman Problem (TSP)

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



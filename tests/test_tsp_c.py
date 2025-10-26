import random,math
import subprocess
import time
import tsplib95 
import pyproj
import matplotlib.pyplot as plt
from tsp_tools import *

def extract_data(path, code, filename, method, distance_fct, coord, graphique, can_len=None):
    """Run the C program and parse its output."""
    if method == "-c":  # canonical
        command = [path + code, "-f", filename, "-c"]
    else:
        command = [path + code, "-f", filename, "-m"] + method.split()

    txt = subprocess.run(command, capture_output=True, text=True)
    lines = txt.stdout.split('\n')

    # Find the data line (skip header)
    data_line = None
    for line in lines:
        if line.strip() and not line.startswith("Instance"):
            data_line = line
            break

    if data_line is None:
        print(f"Erreur: Pas de données trouvées dans la sortie C. stdout: {txt.stdout}")
        return

    parts = [part.strip() for part in data_line.split(';')]

    try:
        if method == "-c" and len(parts) == 5:
            # Format: Instance ; canonical ; Time ; Length ; [Tour]
            nom = parts[0]
            algo = parts[1]
            tps = float(parts[2])
            len_c = float(parts[3])
            tour_str = parts[4]

            # Convert tour string to list
            tour_str_clean = tour_str.strip('[]')
            tour = [int(x.strip()) for x in tour_str_clean.split(',')]

            # Compute canonical length in Python
            len_py = fitness(tour, distance_fct, coord)

            print(f"{nom} ; {algo} ; {tps:.2f} ; {len_c:.2f} ; {len_py:.2f} ; {tour} ; {round(int(len_py)) == round(int(len_c))}")
            graphique(tour, algo, coord)

        elif len(parts) >= 6:
            # Non-canonical (brute force etc.)
            nom = parts[0]
            algo = parts[1]
            tps = float(parts[2])
            len_c = float(parts[3])
            tour_str = parts[5]

            tour_str_clean = tour_str.strip('[]')
            tour = [int(x.strip()) for x in tour_str_clean.split(',')]

            v = valid(tour)
            len_py = fitness(tour, distance_fct, coord)

            print(f"{nom} ; {algo} ; {len_c:.2f} ; {len_py:.2f} ; {tps:.2f} ; {tour} ; {v == 0} ; {abs(len_c - len_py) < 1e-6}")
            graphique(tour, algo, coord)

        else:
            print(f"Erreur: Format de sortie incorrect. Parts: {parts}")

    except Exception as e:
        print(f"Erreur (Python) e = {e} lors du parsing de: {data_line}")


def load_instance(filename):
    """Load TSP instance using tsplib95."""
    instance = load_tsp_file(filename)
    instance_dict = instance.as_name_dict()
    coord = instance_dict['node_coords']
    edge_type = instance_dict['edge_weight_type']
    return instance,coord,edge_type

def test_instance(filename,methods):
    """Run tests for one TSP instance and list of methods."""
    instance, coord, edge_type = load_instance(filename)
    distance_fct, graphique = select_fct(edge_type)
    canonical_len = instance.trace_canonical_tour()

    if methods == ["-c"]:
        print("=== Test Canonique ===")
        print("Instance ; algo ; temps CPU (sec) ; long (C) ; long (Python) ; Tour ; mêmes longueurs")
    else:
        print(f"=== Test {methods[0]} === ")
        print("Instance ; algo ; long (C) ; long (Python) ; temps ; tour ; valid ; mêmes longueurs")

    for m in methods:
        extract_data(path, code, filename, m, distance_fct, coord, graphique, canonical_len)
    print("\n")

def tests_instances_list(instances_file,methods):
    """Run tests for a list of TSP instances."""
    with open(instances_file, 'r') as file:
        for instance in file:
            test_instance(instance.strip(),methods)

# === Configuration ===
filename = "example.tsp"  # example instance
path = "../"              # adapt as needed
code = "tsp"              # name of C executable
methods = ["bf"]          # ["-c"] for canonical mode OR ["bf"] for brute force

# Run tests
tests_instances_list("./instances.txt", methods)
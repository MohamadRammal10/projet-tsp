"""
Soft ad hoc pour tester les programme C de résolution du tsp
VD 8/2025
Adapted for C program output format:
Instance ; Méthode ; Temps CPU (sec) ; Meilleure longueur ; Pire longueur ; Tour optimale ; Pire tournée
"""
import random,math
import subprocess
import time
#import os
#import sys
import tsplib95 # fonctions pour lire les fichiers TSPLIB et faire des calculs
import pyproj # pour les coordonnées géométriques
import matplotlib.pyplot as plt # pour les graphiques
from tsp_tools import *

def extract_data(path,code,filename,method,distance_fct,coord,graphique):
    # interaction avec C avec subprocess
    if method == "-c": # canonical
        command = [path+code, "-f",filename,"-c"]
    else: # autres balises
        command = [path+code, "-f",filename,"-m"]+method.split()
    #print("Commande = ",command) # debug
    txt = subprocess.run(command,capture_output=True,text=True)
    
    # Parse the C program output with the new format
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
    
    # Parse the CSV-style output
    parts = [part.strip() for part in data_line.split(';')]
    
    try:
        if len(parts) >= 6: 
            nom = parts[0]  # Instance name
            algo = parts[1]  # Method
            tps = float(parts[2])  # Time
            length_c = float(parts[3])  # Best length from C
            # parts[4] is worst length - we don't need it
            tour_str = parts[5]  # Best tour string
            
            # Convert tour string to list
            # Remove brackets and split by commas
            tour_str_clean = tour_str.strip('[]')
            tour = [int(x.strip()) for x in tour_str_clean.split(',')]
            
            # Calculate fitness and validity using Python
            l = fitness(tour, distance_fct, coord)
            v = valid(tour)
            
            print(f"{nom} ; {algo} ; {length_c} ; {l} ; {tps} ; {tour} ; {v==0} ; {l==length_c}")
            graphique(tour,algo,coord)
        else:
            print(f"Erreur: Format de sortie incorrect. Parts: {parts}")
            
    except Exception as e:
        print(f"Erreur (Python) e = {e} lors du parsing de: {data_line}")

def load_instance(filename):
    # utilise fcts tsplib95
    instance = load_tsp_file(filename)
    cities = list(instance.get_nodes())
    instance_dict = instance.as_name_dict()
    print(instance)
    coord = instance_dict['node_coords']
    edge_type = instance_dict['edge_weight_type']
    return instance,coord,edge_type

def test_instance(filename,methods):
    # affiche la banière des résultats et appelle les programme C
    instance,coord,edge_type = load_instance(filename)
    distance_fct,graphique = select_fct(edge_type)
    print( "Canonique (Python)",instance.trace_canonical_tour())
    print("Instance ; algo ; long (C) ; long (Python) ; temps ; tour ; valid ; mêmes longueurs")

    for m in methods:
        extract_data(path,code,filename,m,distance_fct,coord,graphique)
    print ("\n")

def tests_instances_list(instances_file,methods):
    # lance le programme C avec toutes les options de methods
    # et tous les fichiers d'instance de instance_file 
    with open(instances_file, 'r') as file:
        for instance in file:
            test_instance(instance.strip(),methods)

# Liste de paramètres.
# opt = tsplib95.load_solution(filename+'.opt.tour') pour charger un pyfichier solution dans python
filename = "example.tsp" # using a smaller instance for brute force
path = "../" # adaptez à votre cas
code = "tsp" # nom du programme C
methods = ["bf"]

#test_instance(filename,methods) # pour un seul appel
tests_instances_list("./instances.txt",methods) # marche si le C marche
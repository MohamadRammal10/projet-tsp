#include <stdio.h>
#include "../include/tsp_io.h"

int main(int argc, char *argv[]) {
    //temp
    if (argc != 2){
        printf("Usage: %s <tsp_file>\n", argv[0]);    
        return -1;
    }
    
    printf("Projet TSP - version initiale\n");    

    TSPInstance instance;
    char *tsp = argv[1]; 
    printf("Reading TSP files %s\n", tsp);
    if (read_tsplib(tsp, &instance) != 0) {
        printf("Error reading TSP file\n");
        return -1;
    }
    
    return 0;
}
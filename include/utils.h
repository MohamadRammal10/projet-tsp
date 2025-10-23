#include <stdio.h>
#include <stdlib.h>

#ifndef UTILS_H
#define UTILS_H

void usage(const char *p);
int parse_args(int argc, char *argv[], const char **filename, int *do_canonical, int *do_bf);
int canonical_mode(TSPGraph *graph, TSPInstance instance);

#endif /* UTILS_H */
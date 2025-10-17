#ifndef DISTANCE_H
#define DISTANCE_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef double (*DistanceFunc)(double x1, double y1, double x2, double y2);

double distance_euc_2d(double x1, double y1, double x2, double y2);
double distance_att(double x1, double y1, double x2, double y2);
double distance_geo(double x1, double y1, double x2, double y2);

#endif
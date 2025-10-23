#include <math.h>
#include "distance.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ===============================
 * 1. EUC_2D distance
 * Formula: round(sqrt((x2-x1)^2 + (y2-y1)^2))
 * =============================== */
double distance_euc_2d(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    return (double)sqrt(dx * dx + dy * dy);
}

/* ===============================
 * 2. ATT distance (pseudo-Euclidean)
 * Formula: rij = sqrt((dx^2 + dy^2)/10)
 *           tij = round(rij)
 *           dij = tij + 1 if tij < rij else tij
 * =============================== */
double distance_att(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    double rij = sqrt((dx * dx + dy * dy) / 10.0);
    int tij = (int)(rij + 0.5);
    return (tij < rij) ? tij + 1 : tij;
}

/* ===============================
 * 3. GEO distance (using latitude/longitude)
 * TSPLIB conversion:
 *   deg = int(coord)
 *   min = coord - deg
 *   rad = PI * (deg + 5.0 * min / 3.0) / 180
 * Formula:
 *   q1 = cos(lon_i - lon_j)
 *   q2 = cos(lat_i - lat_j)
 *   q3 = cos(lat_i + lat_j)
 *   dij = int(RRR * acos(0.5 * ((1+q1)*q2 - (1-q1)*q3)) + 1)
 * RRR = 6378.388
 * =============================== */
static double to_radians(double coord) {
    int deg = (int)coord;
    double min = coord - deg;
    return M_PI * (deg + 5.0 * min / 3.0) / 180.0;
}

double distance_geo(double x1, double y1, double x2, double y2) {
    const double RRR = 6378.388;

    double lat1 = to_radians(x1);
    double lon1 = to_radians(y1);
    double lat2 = to_radians(x2);
    double lon2 = to_radians(y2);

    double q1 = cos(lon1 - lon2);
    double q2 = cos(lat1 - lat2);
    double q3 = cos(lat1 + lat2);

    double dij = RRR * acos(0.5 * ((1.0 + q1)*q2 - (1.0 - q1)*q3)) + 1.0;
    return floor(dij);
}

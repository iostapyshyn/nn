#include <stdlib.h>
#include <math.h>

#include "util.h"

/* Box-Muller algorithm of generating normal distribution using uniform RNG. */
double rand_normal_distribution(double mu, double sigma) {
    static double z0;
    static double z1;

    static double generated = 0;
    generated = !generated;

    /* If the function was previously called,
     * there is no point in generating again.
     * return z1 from the previous pass */
    if (generated) return z1 * sigma + mu;

    double u1, u2;
    do {
        u1 = (double) rand() / RAND_MAX;
        u2 = (double) rand() / RAND_MAX;
    } while (u1 == 0.0); /* u1 cannot be zero */

    z0 = sqrt(-2 * log(u1)) * cos(2*M_PI * u2);
    z1 = sqrt(-2 * log(u1)) * sin(2*M_PI * u2);

    return z0 * sigma + mu;
}

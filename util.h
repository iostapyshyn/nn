#ifndef NN_UTIL_H
#define NN_UTIL_H

#include "neuralnetwork.h"

int nn_writefile(layer *head, const char *filename);
layer *nn_readfile(const char *filename);

double rand_normal_distribution(double mu, double sigma);

#endif

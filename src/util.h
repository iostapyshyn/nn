#ifndef NN_UTIL_H
#define NN_UTIL_H

#include "neuralnetwork.h"

int nn_writefile(const neuralnetwork *nn, const char *filename);
neuralnetwork *nn_readfile(const char *filename);

double rand_normal_distribution(double mu, double sigma);

#endif

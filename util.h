#ifndef NN_UTIL_H
#define NN_UTIL_H

#include "neuralnetwork.h"

int nn_writefile(layer *head, char *filename);
layer *nn_readfile(char *filename);

double rand_normal_distribution(double mu, double sigma);

#endif

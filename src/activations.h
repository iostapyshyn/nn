/**
 * Copyright (c) 2019 Illia Ostapyshyn
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 */

#ifndef NN_ACTIVATIONS_H
#define NN_ACTIVATIONS_H

#ifndef RELU_LEAKY_LEAKAGE
#define RELU_LEAKY_LEAKAGE 0.01
#endif

/* Neuron activation functions and their derivatives. */
enum {
    IDENTITY = 0, STEP, TANH, RELU, RELU_LEAKY, GAUSSIAN, SIGMOID, SOFTPLUS,
    ACTIVATIONS_N /* used as the array size for declaration */
};

double (*activations[ACTIVATIONS_N])(double);
double (*activations_primes[ACTIVATIONS_N])(double);

#endif

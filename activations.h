#ifndef NN_ACTIVATIONS_H
#define NN_ACTIVATIONS_H

/* Neuron activation functions and their derivatives. */
enum {
    IDENTITY = 0, STEP, TANH, RELU, GAUSSIAN, SIGMOID,
    ACTIVATIONS_N /* used as the array size for declaration */
};

double (*activations[ACTIVATIONS_N])(double);
double (*activations_primes[ACTIVATIONS_N])(double);

#endif

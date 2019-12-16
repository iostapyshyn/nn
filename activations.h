#ifndef NN_ACTIVATIONS_H
#define NN_ACTIVATIONS_H

#define RELU_LEAKY_LEAKAGE 0.01

/* Neuron activation functions and their derivatives. */
enum {
    IDENTITY = 0, STEP, TANH, RELU, RELU_LEAKY, GAUSSIAN, SIGMOID, SOFTPLUS,
    ACTIVATIONS_N /* used as the array size for declaration */
};

double (*activations[ACTIVATIONS_N])(double);
double (*activations_primes[ACTIVATIONS_N])(double);

#endif

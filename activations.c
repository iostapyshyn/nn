#include <math.h>

#include "activations.h"

static double activation_identity(double x) { return x; }
static double activation_identity_prime(double x) { return 1; }

static double activation_step(double x) { return x > 0 ? 1 : 0; }
static double activation_step_prime(double x) { return x != 0 ? 0 : INFINITY; }

static double activation_tanh(double x) { return tanh(x); }
static double activation_tanh_prime(double x) { return 1 - activation_tanh(x) * activation_tanh(x); }

static double activation_relu(double x) { return x > 0 ? x : 0; }
static double activation_relu_prime(double x) { return x > 0 ? 1 : 0; }

static double activation_gaussian(double x) { return exp(-x*x); }
static double activation_gaussian_prime(double x) { return -2*x*activation_gaussian(x); }

static double activation_sigmoid(double x) { return 1.0/(1+exp(-x)); }
static double activation_sigmoid_prime(double x) { return activation_sigmoid(x)*(1-activation_sigmoid(x)); }

/* Activation functions. */
double (*activations[])(double) = {
    activation_identity,
    activation_step,
    activation_tanh,
    activation_relu,
    activation_gaussian,
    activation_sigmoid,
};

/* Derivatives of activation functions. */
double (*activations_primes[])(double) = {
    activation_identity_prime,
    activation_step_prime,
    activation_tanh_prime,
    activation_relu_prime,
    activation_gaussian_prime,
    activation_sigmoid_prime,    
};

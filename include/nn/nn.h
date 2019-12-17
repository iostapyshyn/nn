#ifndef NN_NN_H
#define NN_NN_H

enum {
    IDENTITY = 0, STEP, TANH, RELU, RELU_LEAKY, GAUSSIAN, SIGMOID, SOFTPLUS,
    ACTIVATIONS_N /* used as the array size for declaration */
};

typedef struct layer layer;
typedef struct neuralnetwork neuralnetwork;

/* Allocate a new (empty) network with n inputs. */
neuralnetwork *nn_create(int n);
/* Deallocates the network and all allocated data. */
void nn_destroy(neuralnetwork *nn);

/* Adds a new layer to the network with specified number of neurons.
 * Weights can be passed as a matrix stored in a double array.
 * NULL initializes weights randomly and biases with 0. */
void nn_addlayer(neuralnetwork *nn, int nodes, double *weights, double *biases, int activation);

/* Forward propagates a given input through the network.
 * Returns pointer to the output array. */
double *nn_forwardpropagate(neuralnetwork *nn, double *input);

/* Performs forward propagation followed by the backpropagation to teach the network. 
 * Passing learning rate of 0 will not perform back propagation. 
 * Returns mean squared error of the forward pass. */
double nn_backpropagate(neuralnetwork *nn, double *input, double *target, double learningrate);

/* Useful functions for getting the number of inputs/outputs of the network. */
int nn_ninputs(neuralnetwork *nn);
int nn_noutputs(neuralnetwork *nn);


/* File I/O functions. Stores/reads the network as a binary file to keep the double precision. */
int nn_writefile(const neuralnetwork *nn, const char *filename);
neuralnetwork *nn_readfile(const char *filename);

#endif

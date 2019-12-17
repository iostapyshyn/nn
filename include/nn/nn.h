/**
 * @file nn.h
 * @author  Illia Ostapyshyn
 *
 * @section LICENSE
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
 *
 * @section DESCRIPTION
 *
 * The main header file.
 */

#ifndef NN_NN_H
#define NN_NN_H

/**
 * Enumeration of all implemented activation functions.
 */
enum activations {
    IDENTITY = 0, STEP, TANH, RELU, RELU_LEAKY, GAUSSIAN, SIGMOID, SOFTPLUS,
    ACTIVATIONS_N /* used as the array size for declaration */
};

/**
 * Struct representing a neural network.
 */
typedef struct neuralnetwork neuralnetwork;

/**
 * Allocates a new (empty) network. 
 * @param inputs The number of inputs of the new network.
 * @return A pointer to the heap allocated struct. 
 */
neuralnetwork *nn_create(int inputs);

/**
 * Deallocates the network and all allocated data.
 * @param nn The pointer to the network to be deallocated. 
 */
void nn_destroy(neuralnetwork *nn);

/**
 * Adds a new layer to the network with specified number of neurons.
 * @param nn The pointer to the neural network struct.
 * @param nodes Number of neurons/outputs in the layer.
 * @param weights Matrix of the weights stored as an array, i.e. the index in the last dimension changes the fastest.
 * NULL causes weights to be randomly initialized.
 * @param biases Vector of the biases stored like as an array. NULL initializes biases with zeroes.
 * @param activation Activation function index from the enum.
 */
void nn_addlayer(neuralnetwork *nn, int nodes, double *weights, double *biases, int activation);

/**
 * Forward propagates a given input through the network.
 * @param nn The pointer to the neural network struct.
 * @param input Input vector as an array.
 * @return A pointer to the output array.
 */
double *nn_forwardpropagate(const neuralnetwork *nn, const double *input);

/**
 * Performs forward propagation followed by the backpropagation to teach the network. 
 * @param nn The pointer to the neural network struct.
 * @param input Input vector as an array.
 * @param target Target output vector as an array.
 * @param learningrate Learning rate for the pass.
 * Passing learning rate of 0 will not perform the back propagation. 
 * @return Mean squared error of the forward pass.
 */
double nn_backpropagate(const neuralnetwork *nn, const double *input, const double *target, double learningrate);

/**
 * Returns fan-in of the input layer of the network.
 */
int nn_ninputs(const neuralnetwork *nn);
/**
 * Returns fan-out of the output layer of the network.
 */
int nn_noutputs(const neuralnetwork *nn);

/**
 * Stores the network as a binary file (to keep the double precision).
 * @param nn The pointer to the neural network struct.
 * @param filename The relative path to the file. The file will be overwritten/created.
 * @return Positive integer is returned for success.
 */
int nn_writefile(const neuralnetwork *nn, const char *filename);

/**
 * Reads the network from a binary file created by nn_writefile
 * @param filename The relative path to the file. The file will be overwritten/created.
 * @return Pointer to the newly allocated network struct is returned for success.
 * NULL is returned in case of failure. The error message is printed to stderr.
 */
neuralnetwork *nn_readfile(const char *filename);

#endif

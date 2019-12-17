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

#ifndef NN_NEURALNETWORK_H
#define NN_NEURALNETWORK_H

#include "matrix.h"
#include "activations.h"

typedef struct layer {
    matrix *weights; /* MxN matrix, where:
                        * M - number of outputs,
                        * N - number of inputs. */
    matrix *weights_delta; /* weight gradients multiplied by the learning rate */

    matrix *biases; /* biases */
    matrix *biases_delta; /* bias gradients multiplied by the learning rate */
    
    int activation; /* activation function index */

    matrix *net; /* weighted sum of inputs */
    matrix *out; /* net with applied activation function */

    struct layer *prev; /* pointer to the previous layer of the network */
    struct layer *next; /* pointer to the next layer of the network */
} layer;

typedef struct neuralnetwork {
    int inputs;
    int outputs;
    layer *head;
} neuralnetwork;

neuralnetwork *nn_create(int inputs);
void nn_addlayer(neuralnetwork *head, int outputs, double *weights, double *biases,
                 int activation);

int nn_ninputs(neuralnetwork *nn);
int nn_noutputs(neuralnetwork *nn);

double *nn_forwardpropagate(neuralnetwork *nn, double *input);
double nn_backpropagate(neuralnetwork *nn, double *input, double *target, double learningrate);

void nn_destroy(neuralnetwork *nn);

#endif

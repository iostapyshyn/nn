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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "matrix.h"
#include "neuralnetwork.h"
#include "activations.h"
#include "util.h"

static inline int layer_ninputs(layer *layer) {
    return layer->weights->cols;
}

static inline int layer_noutputs(layer *layer) {
    return layer->weights->rows;
}

int nn_ninputs(neuralnetwork *nn) {
    return nn->inputs;
}

int nn_noutputs(neuralnetwork *nn) {
    return nn->outputs;
}

/* Returns output layer of the network. */
static layer *nn_outputlayer(neuralnetwork *nn) {
    layer *current = nn->head;
    while (current->next != NULL)
        current = current->next;
    return current;
}

/* Xavier weights initialization function. Uses truncated gaussian distribution with
 * standart deviation related to the number of inputs and outputs. */
static double xavier_generate(int outputs, int inputs) {
    /* Standart deviation. */
    double sigma = sqrt(2.0 / (outputs+inputs));
    /* Limit. */
    double max = sqrt(6.0 / (outputs+inputs));

    double val = rand_normal_distribution(0, sigma);

    if (val < -max) {
        return -max;
    } else if (val > max) {
        return max;
    } else return val;
}

/* Kaiming weights initialization for ReLU units. */
static double kaiming_generate(int inputs, double a) {
    /* Standart deviation. */
    double sigma = sqrt(2.0 / (inputs*(1+a*a)));

    return rand_normal_distribution(0, sigma);
}

neuralnetwork *nn_create(int ninputs) {
    neuralnetwork *nn = malloc(sizeof(neuralnetwork));
    nn->inputs = ninputs;
    nn->head = 0;
    return nn;
}

/* Allocates and adds a new layer to the linked list,
 * If weights is NULL, the weights will be generated by Xavier weights initialization function.
 * If biases is NULL, initialize biases as 0 vector. */
void nn_addlayer(neuralnetwork *nn, int outputs, double *weights, double *biases, int activation) {
    int inputs = 0;
    layer *new = NULL; /* newly allocated layer struct */
    if (nn->head == NULL) {
        nn->head = calloc(sizeof(layer), 1);
        new = nn->head;

        new->prev = NULL;
        inputs = nn->inputs;

        /* Seed RNG on the first pass. */
        srand(time(NULL));

    } else {
        layer *current = nn->head;
        while (current->next != NULL) current = current->next;

        current->next = calloc(sizeof(layer), 1);
        new = current->next;

        new->prev = current;
        inputs = layer_noutputs(current);
    }

    new->weights = create_matrix(outputs, inputs, weights);
    new->weights_delta = create_matrix(outputs, inputs, NULL);

    /* Populate the weights matrix. */
    if (weights == NULL) {
        for (int i = 0; i < new->weights->rows; i++) {
            for (int j = 0; j < new->weights->cols; j++) {
                switch (activation) {
                case RELU:
                    *matrix_at(new->weights, i, j) = kaiming_generate(inputs, 0); break;
                case RELU_LEAKY:
                    *matrix_at(new->weights, i, j) = kaiming_generate(inputs, RELU_LEAKY_LEAKAGE); break;
                default:
                    *matrix_at(new->weights, i, j) = xavier_generate(outputs, inputs); break;
                }
            }
        }
    }

    new->biases = create_matrix(outputs, 1, biases);
    new->biases_delta = create_matrix(outputs, 1, NULL);

    new->activation = activation;

    new->net = create_matrix(outputs, 1, NULL);
    new->out = create_matrix(outputs, 1, NULL);

    new->next = NULL;
    nn->outputs = outputs;
}

/* Recursive deallocation function. Can be changed to use a loop instead. */
static void layers_destroy(layer *head) {
    if (head != NULL) {
        layers_destroy(head->next);

        matrix_destroy(head->weights);
        matrix_destroy(head->weights_delta);

        matrix_destroy(head->biases);
        matrix_destroy(head->biases_delta);

        matrix_destroy(head->net);
        matrix_destroy(head->out);

        free(head);
    }
}

void nn_destroy(neuralnetwork *nn) {
    layers_destroy(nn->head);
    free(nn);
}

/* Computes the output of the given layer,
 * storing results in the layer struct. */
static void layer_apply(layer *layer, matrix *invec) {
    matrix_product(layer->weights, invec, layer->net);
    matrix_add(layer->net, layer->biases);

    matrix_apply(layer->net, activations[layer->activation], layer->out);
}

/* Forward propagates given input through the network.
 * The output will be stored in the output buffer, if given. */
double *nn_forwardpropagate(neuralnetwork *nn, double *input) {
    if (nn == NULL || nn->head == NULL) return NULL;

    /* Allocate a matrix on stack for the first layer.
     * The out field of the layer is used as input for all the consecutive layers. */
    matrix first = { nn->inputs, 1, input };
    matrix *p = &first;

    int i = 0;
    layer *current = nn->head;
    while (current != NULL) {
        layer_apply(current, p);
        p = current->out;

        current = current->next;
    }

    return p->data;
};

/* Return squared error for the given output and target. */
static double squarederror(double out, double target) {
    return (target - out) * (target - out) / 2;
}

/* Compute dE/dnet of the previous layer for the backpropagation. */
static void nextdelta(matrix **delta, layer *layer) {
    matrix *nextdelta = create_matrix(1, layer_noutputs(layer->prev), NULL);

    /* Transpose delta */
    (*delta)->cols = (*delta)->rows;
    (*delta)->rows = 1;

    matrix_product(*delta, layer->weights, nextdelta);

    /* Transpose newdelta */
    nextdelta->rows = nextdelta->cols;
    nextdelta->cols = 1;

    matrix *doutdnet = create_matrix(layer_noutputs(layer->prev), 1, NULL);
    matrix_apply(layer->prev->net, activations_primes[layer->prev->activation], doutdnet);

    matrix_multiply_elementwise(nextdelta, doutdnet);

    matrix_destroy(doutdnet);
    matrix_destroy(*delta);

    *delta = nextdelta;
}

static void update_weights(const neuralnetwork *nn) {
    layer *p = nn->head;
    while (p != NULL) {
        matrix_add(p->weights, p->weights_delta);
        p = p->next;
    }
}

static void update_biases(const neuralnetwork *nn) {
    layer *p = nn->head;
    while (p != NULL) {
        matrix_add(p->biases, p->biases_delta);
        p = p->next;
    }
}

double nn_backpropagate(neuralnetwork *nn, double *input, double *target, double learningrate) {
    layer *last = nn_outputlayer(nn);
    int outn = nn->outputs; /* quantity of network's outputs */

    double *output = nn_forwardpropagate(nn, input); /* forward propagate to get output */

    double etotal = 0, dEdout[outn];
    for (int i = 0; i < outn; i++) {
        /* Calculate total squared error of the forward pass. */
        etotal += squarederror(output[i], target[i]);
        /* dE/dout = (d/dout)*[1/2*(target-out)^2] = out - target */
        dEdout[i] = output[i] - target[i];
    }

    /* What is the point of backpropagation with 0 learning rate? */
    if (learningrate == 0) return etotal;

    /* Allocate a new matrix containing the dE/dout for each output. */
    matrix *delta = create_matrix(outn, 1, dEdout);

    /* dout/dnet = f'(net) */
    matrix *doutdnet = create_matrix(outn, 1, NULL);
    matrix_apply(last->net, activations_primes[last->activation], doutdnet);

    /* dE/dout * dout/dnet = dE/dnet in delta */
    matrix_multiply_elementwise(delta, doutdnet);

    matrix_destroy(doutdnet);

    layer *current = last;
    while (current->prev != NULL) {
        /* dnet/dWij = output of the prev. layer,
         * as all the terms except outj*Wij in the net summation are treated as constants and
         * therefore vanish after taking derivative. */

        /* Hacky transpose of prev. layer's out matrix for multiplication purposes. */
        matrix dnetdw = { 1, current->prev->out->rows, current->prev->out->data };

        /* Yield the weights' gradients by multiplying dE/dnet by dnet/dWij */
        matrix_product(delta, &dnetdw, current->weights_delta);
        matrix_scalarproduct(current->weights_delta, -learningrate); /* apply the learning rate */

        /* Derivative of net with respect to the biases (dnet/dB) is always 1.
         * Therefore bias gradients are delta * 1:
         * dE/dB = dE/dnet * dnet/dB = dE/dnet = delta */
        memcpy(current->biases_delta->data, delta->data, delta->rows * sizeof(double));
        matrix_scalarproduct(current->biases_delta, -learningrate);

        /* get dE/dnet of the previous layer for the next iteration. */
        nextdelta(&delta, current);

        current = current->prev;
    }

    /* Finally deal with the input layer. */
    matrix dnetdw = { 1, nn->inputs, input };

    /* Yield the weights' gradients by multiplying dE/dnet by dnet/dWij */
    matrix_product(delta, &dnetdw, nn->head->weights_delta);
    matrix_scalarproduct(nn->head->weights_delta, -learningrate);

    /* Store bias learning values. */
    memcpy(current->biases_delta->data, delta->data, delta->rows * sizeof(double));
    matrix_scalarproduct(current->biases_delta, -learningrate);

    update_weights(nn);
    update_biases(nn);

    matrix_destroy(delta);
    return etotal;
}

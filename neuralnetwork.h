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

layer *nn_output(layer *head);

void nn_addlayer(layer **head, int outputs, int inputs, double *weights, double *biases,
                 int activation);

double *nn_forwardpropagate(layer *head, double *input);
double nn_backpropagate(layer *head, double *input, double *target, double learningrate);

double forwardpropagate_error(layer *head, double *input, double *target);

void nn_destroy(layer *head);

static inline int layer_ninputs(layer *layer) {
    return layer->weights->cols;
}

static inline int layer_noutputs(layer *layer) {
    return layer->weights->rows;
}

static inline int nn_ninputs(layer *head) {
    return layer_ninputs(head);
}

static inline int nn_noutputs(layer *head) {
    return layer_noutputs(nn_output(head));
}

#endif

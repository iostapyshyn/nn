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

/* Returns output layer of the network. */
layer *nn_output(layer *head) {
    while (head->next != NULL)
        head = head->next;
    return head;
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

/* Allocates and adds a new layer to the linked list,
 * If weights is NULL, the weights will be generated by Xavier weights initialization function.
 * If biases is NULL, initialize biases as 0 vector. */
void nn_addlayer(layer **head, int outputs, int inputs, double *weights,
                 double *biases, int activation) {
    
    layer *new = NULL; /* newly allocated layer struct */
    if (*head == NULL) {
        *head = calloc(sizeof(layer), 1);
        new = *head;

        new->prev = NULL;

        /* Seed RNG on the first pass. */
        srand(time(NULL));
    
    } else {
        layer *current = *head;
        while (current->next != NULL) current = current->next;

        current->next = calloc(sizeof(layer), 1);
        new = current->next;
        
        new->prev = current;        
    }

    new->weights = create_matrix(outputs, inputs, weights);
    new->weights_delta = create_matrix(outputs, inputs, NULL);

    /* Populate the weights matrix. */
    if (weights == NULL) {    
        for (int i = 0; i < new->weights->rows; i++) {
            for (int j = 0; j < new->weights->cols; j++) {
                double rnd = 2*((double) rand())/RAND_MAX - 1;
                *matrix_at(new->weights, i, j) = xavier_generate(outputs, inputs);
            }
        }
    }
    
    new->biases = create_matrix(outputs, 1, biases);
    new->biases_delta = create_matrix(outputs, 1, NULL);

    new->activation = activation;

    new->net = create_matrix(outputs, 1, NULL);
    new->out = create_matrix(outputs, 1, NULL);
    
    new->next = NULL;
}

/* Recursive deallocation function. Can be changed to use a loop instead. */
void nn_destroy(layer *head) {
    if (head != NULL) {
        nn_destroy(head->next);
        
        matrix_destroy(head->weights);
        matrix_destroy(head->weights_delta);
        matrix_destroy(head->biases);
        matrix_destroy(head->biases_delta);
        matrix_destroy(head->net);
        matrix_destroy(head->out);
        
        free(head);
    }
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
double *nn_forwardpropagate(layer *head, double *input) {    
    assert(head); //!!
    
    int insize = layer_ninputs(head);

    /* Allocate a matrix on stack for the first layer.
     * The out field of the layer is used as input for all the consecutive layers. */
    matrix first = { insize, 1, input };
    matrix *p = &first;
    
    int i = 0;
    layer *current = head;
    while (current != NULL) {
        layer_apply(current, p);

        p = current->out;
        
#if 1 /* Debug print */
        fprintf(stderr, "Output of layer %d:\n[", i++);
        for (int i = 0; i < p->rows; i++)
            fprintf(stderr, "%.2lf ", *matrix_at(p, i, 0));
        puts("]");
#endif

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

static void update_weights(layer *p) {
    while (p != NULL) {
        matrix_add(p->weights, p->weights_delta);        
        p = p->next;
    }
}

double nn_backpropagate(layer *head, double *input, double *target, double learningrate) {    
    layer *last = nn_output(head);
    int outn = layer_noutputs(last); /* quantity of network's outputs */
    
    double *output = nn_forwardpropagate(head, input); /* forward propagate to get output */

    double etotal = 0, dEdout[outn];
    for (int i = 0; i < outn; i++) {
        /* Calculate total squared error of the forward pass. */
        etotal += squarederror(output[i], target[i]);
        /* dE/dout = (d/dout)*[1/2*(target-out)^2] = out - target */
        dEdout[i] = output[i] - target[i];
    }

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

        /* get dE/dnet of the previous layer for the next iteration. */
        nextdelta(&delta, current);

        current = current->prev;
    }

    /* Finally deal with the input layer. */
    matrix dnetdw = { 1, layer_ninputs(head), input };

    /* Yield the weights' gradients by multiplying dE/dnet by dnet/dWij */
    matrix_product(delta, &dnetdw, head->weights_delta);
    matrix_scalarproduct(head->weights_delta, -learningrate);

    matrix_destroy(delta);
    update_weights(head);
    
    return etotal;
}

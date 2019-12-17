#ifndef NN_NN_H
#define NN_NN_H

enum {
    IDENTITY = 0, STEP, TANH, RELU, RELU_LEAKY, GAUSSIAN, SIGMOID, SOFTPLUS,
    ACTIVATIONS_N /* used as the array size for declaration */
};

typedef struct layer layer;
typedef struct neuralnetwork neuralnetwork;

neuralnetwork *nn_create(int inputs);
void nn_addlayer(neuralnetwork *head, int outputs, double *weights, double *biases,
                 int activation);

double *nn_forwardpropagate(neuralnetwork *nn, double *input);
double nn_backpropagate(neuralnetwork *nn, double *input, double *target, double learningrate);

int nn_ninputs(neuralnetwork *nn);
int nn_noutputs(neuralnetwork *nn);

void nn_destroy(neuralnetwork *nn);

/* File I/O functions. */
int nn_writefile(const neuralnetwork *nn, const char *filename);
neuralnetwork *nn_readfile(const char *filename);

#endif

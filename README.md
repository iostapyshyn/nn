# NN
Neural Network with back propagation learning algorithm written in C.

## Usage
The project is intended to be used as a library.
However, there is a demo program `examples/digits.c` included, which uses
the [MNIST database](http://yann.lecun.com/exdb/mnist/)
to train itself and classify images of handwritten digits.

```
git submodule update --init --recursive
cd build
cmake ..
make
```

To run the example:
```
./digits five.png
```

The program will perform the training on it's first run.

### Quickstart
Look into `include/nn/nn.h` for API explanation.
```c
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
```

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

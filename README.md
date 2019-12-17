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
digits five.png
```

The program will perform the training on it's first run.

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

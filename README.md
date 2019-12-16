# NN
Neural Network with back propagation learning algorithm written in C.

## Usage
The project is intended to be used as a library.
However, there are a test program included `digits.c`, which uses
the [MNIST database](http://yann.lecun.com/exdb/mnist/)
to train itself and classify images of handwritten digits.

```
mkdir build
cd build
cmake ..
make
```

From the root directory:
```
build/digits image.png
```

The program will perform the training on it's first run.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

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

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

#include "nn/nn.h"

#define LEARNING_RATE 0.5
#define PIXEL_ROWS 28
#define PIXEL_COLS 28

/* MNIST database is stored in Big Endian. */
#ifndef __BIG_ENDIAN__
#define betoh(x)                                                \
    ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
     | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))
#else
#define betoh(x) (x)
#endif

/* Path to the pretrained network. */
const char *netfile = "mnist/net.nn";

/* Used for MNIST training. */
struct {
    FILE *images;
    FILE *labels;
    double *pixels;
    double *target;
    uint32_t nleft;
} imageset = { NULL, NULL, NULL, NULL, 0 };

/* Closes opened training set. */
void imageset_close() {
    if (imageset.images) fclose(imageset.images);
    if (imageset.labels) fclose(imageset.labels);
    if (imageset.pixels) free(imageset.pixels);
    if (imageset.target) free(imageset.target);
}

/* Opens and reads the necessary values from the MNIST database. */
int imageset_open(const char *images, const char *labels) {
    imageset.images = fopen(images, "rb");
    imageset.labels = fopen(labels, "rb");

    if (!imageset.images || !imageset.labels) {
        imageset_close();
        return 0;
    }

    uint32_t magic_number;
    fread(&magic_number, sizeof(magic_number), 1, imageset.images);
    if (betoh(magic_number) != 0x00000803) {
        fprintf(stderr, "%s: %s: magic number does not match: %X\n", __func__, images, magic_number);
        imageset_close();
        return 0;
    }

    fread(&magic_number, sizeof(magic_number), 1, imageset.labels);
    if (betoh(magic_number) != 0x00000801) {
        fprintf(stderr, "%s: %s: magic number does not match: %X\n", __func__, labels, magic_number);
        imageset_close();
        return 0;
    }

    /* Read number of elements in the database. */
    uint32_t nleft_i, nleft_l;
    fread(&nleft_i, sizeof(uint32_t), 1, imageset.images);
    fread(&nleft_l, sizeof(uint32_t), 1, imageset.labels);
    if (nleft_i != nleft_l) {
        imageset_close();
        return 0;
    }
    
    imageset.nleft = betoh(nleft_i);

    /* Skip image dimensions, we already know it's PIXEL_ROWS x PIXEL_COLS */
    fseek(imageset.images, 2*sizeof(uint32_t), SEEK_CUR);

    imageset.pixels = malloc(PIXEL_ROWS * PIXEL_COLS * sizeof(double));
    imageset.target = malloc(10 /* digits */ * sizeof(double));
    if (!imageset.pixels || !imageset.target) {
        imageset_close();
        return 0;        
    }

    return 1;
}

/* Loads next image data into the imageset struct. */
int imageset_next() {
    assert(imageset.images);

    if (imageset.nleft <= 0)
        return -1;

    uint8_t *rawpixels = malloc(PIXEL_ROWS*PIXEL_COLS);
    if (fread(rawpixels, 1, PIXEL_ROWS*PIXEL_COLS, imageset.images) \
        < PIXEL_ROWS * PIXEL_COLS) {
        free(rawpixels);
        return -1;
    }

    /* Normalize brightness values between [0, 1] */
    for (int i = 0; i < PIXEL_ROWS * PIXEL_COLS; i++) {
        imageset.pixels[i] = rawpixels[i] / 255.0;
    }

    free(rawpixels);

    /* Read label and create target output. */
    uint8_t label;
    if (fread(&label, 1, 1, imageset.labels) < 1 || label < 0 || label > 9) {
        return -1;
    }

    for (int i = 0; i < 10; i++) {
        if (i == label)
            imageset.target[i] = 1.00;
        else imageset.target[i] = 0.00;
    }

    imageset.nleft--;
    return label;
}

/* Trains the network and saves it to the path. */
int train() {
    neuralnetwork *nn = nn_create(PIXEL_ROWS*PIXEL_COLS);
    if (!imageset_open("mnist/train-images-idx3-ubyte", "mnist/train-labels-idx1-ubyte")) {
        return -1;
    }

    nn_addlayer(nn, 300, NULL, NULL, SIGMOID);
    nn_addlayer(nn, 10, NULL, NULL, SIGMOID);
    
    int label;
    while ((label = imageset_next()) != -1) {
        double Etotal = nn_backpropagate(nn, imageset.pixels, imageset.target, LEARNING_RATE);
        printf("Etotal = %lf | %d\n", Etotal, imageset.nleft);
    }

    nn_writefile(nn, netfile);
    
    imageset_close();
    nn_destroy(nn);

    return 0;
}

int main(int argc, char *argv[]) {
    /* Try to open already pre-trained network if exists. Create one if necessary. */
    if (access(netfile, F_OK) == -1) {
        int exit_code;
        if ((exit_code = train()) != -1) {
            fprintf(stderr, "The network has been successfully trained. Please run the program again.\n");
        } else fprintf(stderr, "Unable to train the network. Terminating...\n");
        return exit_code;
    }

    if (argc != 2) {
        fprintf(stderr, "Usage: %s image\n", argv[0]);
        return 1;
    }

    /* Do not change this. */
    const int bytes_per_pixel = 1;
    
    int x, y, n;
    uint8_t *image = stbi_load(argv[1], &x, &y, NULL, bytes_per_pixel);
    if (image == NULL) {
        fprintf(stderr, "Unable to load image %s\n", argv[1]);
        return 1;
    }

    /* Resize the image to match the number of inputs. */
    uint8_t raw[PIXEL_ROWS*PIXEL_COLS];
    if (x != PIXEL_ROWS || y != PIXEL_COLS) {
        stbir_resize_uint8(image, x, y, 0, raw, PIXEL_ROWS, PIXEL_COLS, 0, bytes_per_pixel);
    } else memcpy(raw, image, PIXEL_ROWS*PIXEL_COLS);

    /* Read the pretrained network. */
    neuralnetwork *nn = nn_readfile(netfile);

    double input[PIXEL_ROWS*PIXEL_COLS];
    for (int i = 0; i < PIXEL_ROWS*PIXEL_COLS; i++) {
        input[i] = (double)(255-raw[i]) / 255.0;
    }

    double *output = nn_forwardpropagate(nn, input);

    printf("Neural network output:\n[ ");
    int k = 0;
    for (int i = 0; i < nn_noutputs(nn); i++) {
        printf("%.3lf ", output[i]);
        if (output[i] > output[k]) k = i;
    }
    printf("]\n => Guess: '%d'\n", k);
    
    stbi_image_free(image);
    nn_destroy(nn);    
    return 0;
}

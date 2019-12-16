#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "neuralnetwork.h"

/* Box-Muller algorithm of generating normal distribution using uniform RNG. */
double rand_normal_distribution(double mu, double sigma) {
    static double z0;
    static double z1;

    static double generated = 0;
    generated = !generated;

    /* If the function was previously called,
     * there is no point in generating again.
     * return z1 from the previous pass */
    if (generated) return z1 * sigma + mu;

    double u1, u2;
    do {
        u1 = (double) rand() / RAND_MAX;
        u2 = (double) rand() / RAND_MAX;
    } while (u1 == 0.0); /* u1 cannot be zero */

    z0 = sqrt(-2 * log(u1)) * cos(2*M_PI * u2);
    z1 = sqrt(-2 * log(u1)) * sin(2*M_PI * u2);

    return z0 * sigma + mu;
}

int nn_writefile(layer *start, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror(__func__);
        return 0;
    }

    while (start != NULL) {
        int outputs = layer_noutputs(start);
        int inputs = layer_ninputs(start);

        int acc_written = 0;

        /* Data written for each layer:
         * 1) Number of outputs
         * 2) Number of inputs
         * 3) Weights matrix
         * 4) Biases matrix
         * 5) Activation function */
        
        acc_written += fwrite(&outputs, sizeof(outputs), 1, file); /* Num. of outputs */
        acc_written += fwrite(&inputs, sizeof(inputs), 1, file); /* Num. of inputs */
        acc_written += fwrite(start->weights->data, sizeof(double),
                              outputs * inputs, file); /* Write weights matrix */
        acc_written += fwrite(start->biases->data, sizeof(double), outputs, file); /* Write biases matrix */
        
        acc_written += fwrite(&start->activation, sizeof(int), 1, file);

        if (acc_written < (2 + outputs * inputs + outputs + 1)) {
            perror(__func__);

            fclose(file);
            return 0;
        }

        start = start->next;
    }

    fclose(file);
    
    return 1;
}

layer *nn_readfile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror(__func__);
        return NULL;
    }

    layer *head = NULL;
    while (1) {
        int acc_read = 0; /* counter of elements read */
        int outputs, inputs;
        acc_read += fread(&outputs, sizeof(int), 1, file);
        acc_read += fread(&inputs, sizeof(int), 1, file);

        if (acc_read >= 2) {
            double *weights_data = malloc(outputs*inputs * sizeof(double));
            double *biases_data = malloc(outputs * sizeof(double));
            int activation;
            if (weights_data && biases_data) {
                /* Successful allocation. */
                acc_read += fread(weights_data, sizeof(double), outputs*inputs, file);
                acc_read += fread(biases_data, sizeof(double), outputs, file);
                acc_read += fread(&activation, sizeof(int), 1, file);
            
                if (acc_read >= (2 + outputs*inputs + outputs + 1)) {                
                    /* Total success */
                    nn_addlayer(&head, outputs, inputs, weights_data, biases_data, activation);
                    
                    free(weights_data);
                    free(biases_data);
                
                    continue;
                }
            }
            
            /* Fail */
            free(weights_data);
            free(biases_data);
        }

        /* In case we finished reading every layer there should be no data left.
         * Otherwise, report error. */
        if (acc_read > 0) {
            if (feof(file)) {
                fprintf(stderr, "%s: Unexpected EOF\n", __func__);
            } else perror(__func__); /* Will display I/O as well as ENOMEM error. */

            nn_destroy(head);
            head = NULL;
        }

        break;
    }

    fclose(file);
    return head;
}

#include <stdio.h>

#include "neuralnetwork.h"

/* TODO:
 * Include biases in the back propagation algorithm.
 * Allow to store neural network as a file.
 */

#define LEARNING_RATE 0.5

int main() {
    layer *head = NULL;

/*
    nn_add(&head, 12, 4, NULL, NULL, SIGMOID);
    for (int i = 0; i < 3; i++) {
        nn_add(&head, 12, 12, NULL, NULL, SIGMOID);
    }
    nn_add(&head, 4, 12, NULL, NULL, SIGMOID);
*/

    nn_addlayer(&head, 2, 2, (double[]) {
            0.15, 0.20,
            0.25, 0.30,
        }, (double[]) { 0.35, 0.35 }, SIGMOID);

    nn_addlayer(&head, 2, 2, (double[]) {
            0.40, 0.45,
            0.50, 0.55,
        }, (double[]) { 0.60, 0.60, }, SIGMOID);
    
    double input[layer_ninputs(head)];
    double target[layer_noutputs(nn_output(head))];

    for (;;) {
        printf("Input? ");
        for (int i = 0; i < layer_ninputs(head); i++) {
            scanf("%lf", &input[i]);
        }
        printf("Target? ");
        for (int i = 0; i < layer_noutputs(nn_output(head)); i++) {
            scanf("%lf", &target[i]);
        }

        printf("Etotal = %lf\n", nn_backpropagate(head, input, target, LEARNING_RATE));
    }
    
    nn_destroy(head);  
    return 0;
}

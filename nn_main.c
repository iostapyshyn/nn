#include <stdio.h>
#include <unistd.h>

#include "neuralnetwork.h"
#include "util.h"

#define LEARNING_RATE 0.5

int main(int argc, char *argv[]) {
#if 0

    layer *head = NULL;
    
    nn_addlayer(&head, 2, 2, (double[]) {
            0.15, 0.20,
            0.25, 0.30,
        }, (double[]) { 0.35, 0.35 }, SIGMOID);

    nn_addlayer(&head, 2, 2, (double[]) {
            0.40, 0.45,
            0.50, 0.55,
        }, (double[]) { 0.60, 0.60, }, SIGMOID);

    
    nn_writefile(head, "test.nn");

#else

    layer *head = nn_readfile("test.nn");
    if (head == NULL) {
        return 1;
    }
    
    double input[nn_ninputs(head)];
    double target[nn_noutputs(head)];
    
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
    

#endif

    nn_destroy(head);  
    return 0;
}

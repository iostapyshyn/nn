#ifndef NN_MATRIX_H
#define NN_MATRIX_H

typedef struct {
    int rows, cols;
    double *data;
} matrix;

matrix *create_matrix(int rows, int cols, double *data);

void matrix_product(matrix *A, matrix *B, matrix *out);
void matrix_add(matrix *m, matrix *B);
void matrix_apply(matrix *m, double (*op)(double), matrix *result);

void matrix_scalarproduct(matrix *m, double scalar);
void matrix_subtract(matrix *m, matrix *B);
void matrix_multiply_elementwise(matrix *m, matrix *B);

void matrix_destroy(matrix *m);

static inline double *matrix_at(matrix *m, int row, int col) {
    return &m->data[row*m->cols + col];
}

#endif

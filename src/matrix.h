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

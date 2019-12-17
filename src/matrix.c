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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "matrix.h"
#include "util.h"

/* Allocates new matrix on heap. */
matrix *create_matrix(int rows, int cols, double *data) {
    matrix *new = malloc(sizeof(matrix));
    if (new == NULL) {
        /* Allocation failed, there is no point in continuing. */
        perror(__func__);
        assert(new != NULL);
    }
    
    new->rows = rows;
    new->cols = cols;
    new->data = malloc(rows*cols * sizeof(double));
    if (new->data == NULL) {
        free(new);
        perror(__func__);
        assert(new->data != NULL);
    }
    
    /* Copy data as the underlying array, of fill with zeroes. */
    if (data) {
        memcpy(new->data, data, rows*cols * sizeof(data));
    } else memset(new->data, 0, rows*cols * sizeof(data));

    return new;
}

void matrix_apply(matrix *m, double (*f)(double), matrix *result) {
    /* Result matrix should be of the same dimensions as input. */
    assert(m->rows == result->rows && m->cols == result->cols);

    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            *matrix_at(result, i, j) = f(*matrix_at(m, i, j));
        }
    }
}

void matrix_add(matrix *mat, matrix *B) {
    /* Matrices should have equal dimensions. */
    assert(mat->rows == B->rows && mat->cols == B->cols);

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            *matrix_at(mat, i, j) += *matrix_at(B, i, j);
        }
    }
}

void matrix_scalarproduct(matrix *mat, double scalar) {
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            *matrix_at(mat, i, j) *= scalar;
        }
    }        
}

void matrix_multiply_elementwise(matrix *mat, matrix *B) {
    /* Matrices should have equal dimensions. */
    assert(mat->rows == B->rows && mat->cols == B->cols);

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            *matrix_at(mat, i, j) *= *matrix_at(B, i, j);
        }
    }    
}

void matrix_product(matrix *a, matrix *b, matrix *res) {
    /* Check inputs' dimensions. */
    assert(a->cols == b->rows);

    /* Check result matrix dimensions. */
    assert(res->rows == a->rows && res->cols == b->cols);
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            double sum = 0;
            for (int k = 0; k < a->cols; k++) {
                sum += *matrix_at(a, i, k) * *matrix_at(b, k, j);
            }
            *matrix_at(res, i, j) = sum;
        }
    }
}

void matrix_destroy(matrix *m) {
    if (!m) return;
    
    free(m->data);
    free(m);
}

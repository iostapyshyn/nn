#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "matrix.h"

/* Allocates new matrix on heap. */
matrix *create_matrix(int rows, int cols, double *data) {
    matrix *m = malloc(sizeof(matrix));
    assert(m); // !!
    
    m->rows = rows;
    m->cols = cols;
    m->data = malloc(rows*cols * sizeof(double));

    assert(m->data); // !!
    
    if (data) {
        memcpy(m->data, data, rows*cols * sizeof(data));
    } else memset(m->data, 0, rows*cols * sizeof(data));

    return m;
}

void matrix_apply(matrix *m, double (*f)(double), matrix *result) {
    assert(m->rows == result->rows && m->cols == result->cols);

    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            *matrix_at(result, i, j) = f(*matrix_at(m, i, j));
        }
    }
}

void matrix_add(matrix *mat, matrix *B) {
    assert(mat->rows == B->rows &&
           mat->cols == B->cols);

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            *matrix_at(mat, i, j) += *matrix_at(B, i, j);
        }
    }
}

void matrix_scalarproduct(matrix *matrix, double scalar) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            *matrix_at(matrix, i, j) *= scalar;
        }
    }        
}

void matrix_multiply_elementwise(matrix *mat, matrix *B) {
    assert(mat->rows == B->rows &&
           mat->cols == B->cols);

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            *matrix_at(mat, i, j) *= *matrix_at(B, i, j);
        }
    }    
}

void matrix_product(matrix *a, matrix *b, matrix *res) {
    assert(a->cols == b->rows); //!!

    assert(res->rows == a->rows); //!!
    assert(res->cols == b->cols); //!!
    
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
    free(m->data);
    free(m);
}

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>

#include "ast.h"

typedef struct {
    double complex *amps;   // size 2^n, from <complex.h>
    size_t n_qubits;
} statevec_t;

#endif // !CODEGEN_H
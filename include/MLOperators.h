//
// Created by lchenke on 04.04.23.
//

#ifndef TEST_MNIST_MLOPERATORS_H
#define TEST_MNIST_MLOPERATORS_H

#include <iostream>
#include <math.h>

#include "openfhe.h"

using namespace lbcrypto;

Ciphertext<DCRTPoly> calculateMean(Ciphertext<DCRTPoly> input, CryptoContext<DCRTPoly> context, unsigned int N);

Ciphertext<DCRTPoly> batchNorm(Ciphertext<DCRTPoly> input,
                               CryptoContext<DCRTPoly> context,
                               unsigned int polyDegree,
                               double xMin,
                               double xMax,
                               unsigned int N,
                               double epsilon,
                               std::vector<double> gamma,
                               std::vector<double> beta);

#endif //TEST_MNIST_MLOPERATORS_H

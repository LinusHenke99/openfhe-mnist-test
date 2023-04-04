//
// Created by lchenke on 04.04.23.
//

#ifndef TEST_MNIST_MLOPERATORS_H
#define TEST_MNIST_MLOPERATORS_H

#include <iostream>

#include "openfhe.h"

using namespace lbcrypto;

Ciphertext<DCRTPoly> calculateMean(Ciphertext<DCRTPoly> input, CryptoContext<DCRTPoly> context, unsigned int N);

Ciphertext<DCRTPoly> batchNorm(Ciphertext<DCRTPoly> input, unsigned int polyDegree, double xMin, double xMax);

#endif //TEST_MNIST_MLOPERATORS_H

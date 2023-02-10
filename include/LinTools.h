//
// Created by lchenke on 08.02.23.
//

#ifndef TEST_MNIST_LINTOOLS_H
#define TEST_MNIST_LINTOOLS_H

#include <vector>

#include "openfhe.h"

using namespace lbcrypto;


/*          Plaintext matrix transposing                                                                              */
std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> matrix);



/*          Generating a matrix including the vectors of matrix diagonals                                             */
std::vector<std::vector<double>> diagonal_transformation(std::vector<std::vector<double>> matrix);

/*          Product of ciphertext vector and matrix                                                                   */
Ciphertext<DCRTPoly> matrix_multiplication(std::vector<std::vector<double>> matrix, Ciphertext<DCRTPoly> vector);

#endif //TEST_MNIST_LINTOOLS_H

//
// Created by lchenke on 08.02.23.
//

#ifndef TEST_MNIST_LINTOOLS_H
#define TEST_MNIST_LINTOOLS_H

#include <vector>
#include <bitset>

#include "MatrixFormatting.h"
#include "openfhe.h"

using namespace lbcrypto;


/*          Generating a matrix including the vectors of matrix diagonals                                             */
std::vector<std::vector<double>> diagonal_transformation(std::vector<std::vector<double>> matrix);




/*          Product of ciphertext vector and matrix                                                                   */
Ciphertext<DCRTPoly> matrix_multiplication(
        std::vector<std::vector<double>> matrix,
        Ciphertext<DCRTPoly> vector,
        bool packing = true,
        int numRowsPrevMatrix=1,
        bool masking = true,
        bool transposing = true
);


Ciphertext<DCRTPoly> matrix_multiplication_diagonals(std::vector<std::vector<double>> matrix, Ciphertext<DCRTPoly> vector);


std::vector<double> plain_matrix_multiplication(std::vector<std::vector<double>> matrix, std::vector<double> vector);


std::vector<double> plain_addition(std::vector<double> a, std::vector<double> b);

#endif //TEST_MNIST_LINTOOLS_H

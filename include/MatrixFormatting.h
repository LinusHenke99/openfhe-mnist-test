#ifndef TEST_MNIST_MATRIXFORMATTING_H
#define TEST_MNIST_MATRIXFORMATTING_H

#include <vector>
#include <math.h>
#include <cstdint>


std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> matrix);


unsigned int next_power2 (unsigned int n);


std::vector<double> flattenMatrix(std::vector<std::vector<double>> matrix, bool direction = true);


std::vector<std::vector<double>> resizeMatrix(std::vector<std::vector<double>> matrix, size_t numCols, size_t numRows);


unsigned int find_n1(uint32_t batchSize);


#endif //TEST_MNIST_MATRIXFORMATTING_H

//
// Created by lchenke on 16.02.23.
//

#ifndef TEST_MNIST_KEYGEN_H
#define TEST_MNIST_KEYGEN_H

#include <vector>
#include <set>
#include <math.h>
#include <iostream>

#include "MatrixFormatting.h"


std::vector<int> genRotations (
        std::vector<std::vector<std::vector<double>>> matrices,
        bool packing = true,
        int numRowsPrevMatrix = 1
);


#endif //TEST_MNIST_KEYGEN_H

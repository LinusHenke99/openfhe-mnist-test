//
// Created by lchenke on 01.03.23.
//

#ifndef TEST_MNIST_LOADMODEL_H
#define TEST_MNIST_LOADMODEL_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>


std::vector<std::vector<double>> loadMatrix (std::string filename);


std::vector<double> loadBias (std::string filename);


#endif //TEST_MNIST_LOADMODEL_H

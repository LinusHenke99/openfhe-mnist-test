#ifndef TEST_MNIST_LOADIMAGE_H
#define TEST_MNIST_LOADIMAGE_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>


/*
 *          Function to read image binaries
 */
std::vector<double> load_image(std::string pathToFile);

#endif //TEST_MNIST_LOADIMAGE_H

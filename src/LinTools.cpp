#include "LinTools.h"


std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> matrix) {
    std::vector<std::vector<double>> result;

    for(unsigned int i=0; i<matrix[0].size(); i++) {
        std::vector<double> row(matrix.size());
        for (unsigned int j=0; j<matrix.size(); j++) {
            row[j] = matrix[j][i];
        }
        result.push_back(row);
    }

    return result;
}

#include "MatrixFormatting.h"


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


std::vector<double> flattenMatrix(std::vector<std::vector<double>> matrix, bool direction) {
    std::vector<double> result;

    if (!direction)
        matrix = transpose(matrix);

    for (auto row : matrix)
        for (auto entry : row)
            result.push_back(entry);

    return result;
}


unsigned int next_power2 (unsigned int n) {
    return (n==0 || n==1) ? 1 : 1 << (unsigned int) std::ceil(log2(n));
}


std::vector<std::vector<double>> resizeMatrix(std::vector<std::vector<double>> matrix, size_t numRows, size_t numCols) {

    for (auto &row : matrix)
        row.resize(numCols, 0);

    const std::vector<double> zeros(numCols, .0);
    matrix.resize(numRows, zeros);

    return matrix;
}

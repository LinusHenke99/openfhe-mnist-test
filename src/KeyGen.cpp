#include "KeyGen.h"


std::vector<int> genRotations (
        std::vector<std::vector<std::vector<double>>> matrices,
        bool packing,
        int numRowsPrevMatrix
) {
    std::set<int> resultSet;


    for (auto matrix : matrices) {
        size_t ogNumRows = matrix.size();
        size_t ogNumCols = matrix[0].size();

        size_t numRows = next_power2(ogNumRows);
        size_t numCols = packing ? next_power2(ogNumCols) : next_power2(numRowsPrevMatrix);

        for (size_t i=0; i<log2(ogNumCols); i++) {
            resultSet.insert(-((packing ? numRows : 1) << i));
            resultSet.insert((packing ? 1 : numCols) << i);
        }

        for (size_t i=0; i<ogNumCols; i++)
            resultSet.insert(i * (numRows - 1));
    }


    std::cout << "Copying" << std::endl;
    std::cout << resultSet.size() << std::endl;
    std::vector<int> result(resultSet.size());

    std::copy(resultSet.begin(), resultSet.end(), result.begin());

    return result;
}

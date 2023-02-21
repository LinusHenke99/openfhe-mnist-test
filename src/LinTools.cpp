#include "LinTools.h"


std::vector<std::vector<double>> diagonal_transformation(std::vector<std::vector<double>> matrix) {
    std::vector<std::vector<double>> result;

    for (unsigned int i=0; i< matrix.size(); i++) {
        std::vector<double> row;
        for (unsigned int j = 0; j < matrix[0].size(); j++) {
            unsigned int colIndex = (j + i) % matrix[0].size();
            unsigned int rowIndex = j % matrix.size();

            row.push_back(matrix[rowIndex][colIndex]);
        }
        result.push_back(row);
    }

    return result;
}


std::vector<double> rotate_plain(std::vector<double> vector, int index) {
    std::vector<double> result(vector.size());


    if(index == 0)
        return vector;
    else {
        if (index < 0)
            index = vector.size() + index % vector.size();

        for (unsigned int i = 0; i < vector.size(); i++) {
            unsigned int newIndex = (i + index) % vector.size();
            result[i] = vector[newIndex];
        }
    }

    return result;
}


Ciphertext<DCRTPoly> matrix_multiplication_diagonals (std::vector<std::vector<double>> matrix, Ciphertext<DCRTPoly> vector) {
    auto context = vector->GetCryptoContext();
    uint32_t batchSize = vector->GetEncodingParameters()->GetBatchSize();

    matrix = resizeMatrix(matrix, batchSize, batchSize);
    auto diagonals = diagonal_transformation(matrix);

    unsigned int n1 = find_n1(batchSize);
    unsigned int n2 = batchSize / n1;

    Plaintext pl = context->MakeCKKSPackedPlaintext(diagonals[0]);
    Ciphertext<DCRTPoly> subResult = context->EvalMult(pl, vector);

    for (unsigned int j=1; j<n1; j++) {
        pl = context->MakeCKKSPackedPlaintext(diagonals[j]);
        subResult += context->EvalMult(pl, context->EvalRotate(vector, j));
    }

    Ciphertext<DCRTPoly> result = subResult;

    for (unsigned int k=1; k<n2; k++) {
        pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k*n1], k*n1));
        subResult = context->EvalMult(pl, vector);

        for (unsigned int j=1; j<n1; j++) {
            pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k*n1 + j], k*n1));
            subResult += context->EvalMult(pl, context->EvalRotate(vector, j));
        }

        result += context->EvalRotate(subResult, k*n1);
    }

    return result;
}

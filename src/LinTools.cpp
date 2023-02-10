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


std::vector<std::vector<double>> diagonal_transformation(std::vector<std::vector<double>> matrix) {
    std::vector<std::vector<double>> result;

    for (int j=0; j<(int) matrix.size(); j++) {
        std::vector<double> row;
        for (int i = 0; i < (int) matrix[0].size(); i++) {
            int index = (j + i) % matrix.size();
            row.push_back(matrix[index][i]);
        }
        result.push_back(row);
    }

    return result;
}


Ciphertext<DCRTPoly> matrix_multiplication(std::vector<std::vector<double>> matrix, Ciphertext<DCRTPoly> vector) {
    CryptoContext<DCRTPoly> context = vector->GetCryptoContext();

    auto diags = diagonal_transformation(matrix);
    Plaintext diagsPL = context->MakeCKKSPackedPlaintext(diags[0]);

    Ciphertext<DCRTPoly> result = context->EvalMult(diagsPL, vector);

    for (uint32_t i=1; i<diags.size(); i++) {
        Plaintext pl = context->MakeCKKSPackedPlaintext(diags[i]);
        result +=  context->EvalMult(pl, context->EvalRotate(vector, i));
    }

    return result;
}

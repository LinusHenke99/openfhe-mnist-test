#include "LinTools.h"


std::vector<std::vector<double>> diagonal_transformation(std::vector<std::vector<double>> matrix) {
    std::vector<std::vector<double>> result;

    for (unsigned int i=0; i< matrix.size(); i++) {
        std::vector<double> row;
        for (unsigned int j = 0; j < matrix[0].size(); j++) {
            unsigned int colIndex = j;
            unsigned int rowIndex = (j + i) % matrix.size();

            row.push_back(matrix[rowIndex][colIndex]);
        }
        result.push_back(row);
    }

    return result;
}


Ciphertext<DCRTPoly> matrix_multiplication(
        std::vector<std::vector<double>> matrix,
        Ciphertext<DCRTPoly> vector,
        bool packing,
        int numRowsPrevMatrix,
        bool masking,
        bool transposing
) {
    CryptoContext<DCRTPoly> cryptoContext = vector->GetCryptoContext();

    // Store original matrix size.
    size_t ogNumRows = matrix.size();
    size_t ogNumCols = matrix[0].size();

    // Pad and flatten the matrix.
    size_t numRows = next_power2(ogNumRows);
    size_t numCols = packing ? next_power2(ogNumCols) : next_power2(numRowsPrevMatrix);
    matrix = resizeMatrix(matrix, numRows, numCols);
    std::vector<double> matrixFlat = flattenMatrix(matrix, !packing);
    Plaintext matrixFlatP = cryptoContext->MakeCKKSPackedPlaintext(matrixFlat);

    // Pad and repeat the vector.
    for (size_t i = 0; i < log2(ogNumCols); i++)
        vector = cryptoContext->EvalAdd(vector, cryptoContext->EvalRotate(vector, -((packing ? numRows : 1) << i)));

    // Multiply and sum (the result is stored in the first row of the matrix).
    Ciphertext<DCRTPoly> prod = cryptoContext->EvalMult(vector, matrixFlatP);
    for (size_t i = 0; i < log2(numRows); i++)
        prod = cryptoContext->EvalAdd(prod, cryptoContext->EvalRotate(prod, (packing ? 1 : numCols) << i));

    // Mask out the result.
    if (!(packing && transposing) && masking) {
        std::vector<int64_t> mask;
        if (packing) {
            for (size_t i = 0; i < numCols; i++)
                for (size_t j = 0; j < numRows; j++)
                    if (j == 0 && i < ogNumCols)
                        mask.push_back(1);
                    else
                        mask.push_back(0);
        } else {
            mask.insert(mask.end(), ogNumCols, 1);
        }
        Plaintext maskP = cryptoContext->MakePackedPlaintext(mask);
        prod = cryptoContext->EvalMult(prod, maskP);
    }

    // Transpose the result.
    if (packing && transposing) {
        const std::vector<double> ZERO = {.0};
        const Plaintext ZERO_PLAINTEXT = cryptoContext->MakeCKKSPackedPlaintext(ZERO);
        std::vector<double> mask = {1.};
        Plaintext maskP = cryptoContext->MakeCKKSPackedPlaintext(mask);
        Ciphertext<DCRTPoly> res = cryptoContext->EvalAdd(
                ZERO_PLAINTEXT,
                cryptoContext->EvalMult(
                        prod,
                        maskP));
        mask.insert(mask.begin(), 0);

        for (size_t i = 1; i < ogNumCols; i++) {
            maskP = cryptoContext->MakeCKKSPackedPlaintext(mask);
            res = cryptoContext->EvalAdd(
                    res,
                    cryptoContext->EvalMult(
                            cryptoContext->EvalRotate(
                                    prod,
                                    i * (numRows - 1)),
                            maskP));
            mask.insert(mask.begin(), 0);
        }
        prod = res;
    }

    return prod;
}


Ciphertext<DCRTPoly> matrix_multiplication_diagonals (std::vector<std::vector<double>> matrix, Ciphertext<DCRTPoly> vector) {
    auto diagonals = diagonal_transformation(matrix);
    auto context = vector->GetCryptoContext();
    uint32_t batchSize = vector->GetEncodingParameters()->GetBatchSize();

    Plaintext pl = context->MakeCKKSPackedPlaintext(diagonals[0]);
    Ciphertext<DCRTPoly> result = context->EvalMult(pl, vector);

    for (unsigned int i=1; i<batchSize; i++)
        result = context->EvalAdd(result, context->EvalRotate(vector, i));

    return result;
}

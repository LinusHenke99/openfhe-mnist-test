#include "LinTools.h"


std::vector<double> rotate_plain(const std::vector<double>& vector, int index) {
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


Ciphertext<DCRTPoly> matrix_multiplication(
        std::vector<std::vector<double>> matrix,
        const Ciphertext<DCRTPoly>& vector,
        CryptoContext<DCRTPoly> context,
        bool parallel
) {
    return parallel ?
           matrix_multiplication_parallel(matrix, vector, context):
           matrix_multiplication_diagonals(matrix, vector, context);
}


Ciphertext<DCRTPoly> matrix_multiplication_diagonals (std::vector<std::vector<double>> matrix, const Ciphertext<DCRTPoly>& vector, CryptoContext<DCRTPoly> context) {
    uint32_t batchSize = vector->GetEncodingParameters()->GetBatchSize();

    //  Resizing matrix to the contexts batchsize and getting a matrix in diagonal order
    matrix = resizeMatrix(matrix, batchSize, batchSize);
    auto diagonals = diagonal_transformation(matrix);

    //  Finding the optimal configuration for
    unsigned int n1 = find_n1(batchSize);
    unsigned int n2 = batchSize / n1;

    Plaintext pl = context->MakeCKKSPackedPlaintext(diagonals[0]);
    Ciphertext<DCRTPoly> subResult = context->EvalMult(pl, vector);

    auto cipherPrecompute = context->EvalFastRotationPrecompute(vector);
    uint32_t M = 2 * context->GetRingDimension();

    std::vector<Ciphertext<DCRTPoly>> rotCache;
    for (unsigned int j=1; j<n1; j++) {
        pl = context->MakeCKKSPackedPlaintext(diagonals[j]);
        Ciphertext<DCRTPoly> rotation = context->EvalFastRotation(vector, j, M, cipherPrecompute);
        rotCache.push_back(rotation);
        subResult += context->EvalMult(pl, rotation);
    }

    Ciphertext<DCRTPoly> result = subResult;

    for (unsigned int k=1; k<n2; k++) {
        pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k*n1], -k*n1));
        subResult = context->EvalMult(pl, vector);

        for (unsigned int j=1; j<n1; j++) {
            pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k*n1 + j], -k*n1));
            subResult += context->EvalMult(pl, rotCache[j-1]);
        }

        result += context->EvalRotate(subResult, k*n1);
    }

    return result;
}


Ciphertext<DCRTPoly> matrix_multiplication_parallel(std::vector<std::vector<double>> matrix, const Ciphertext<DCRTPoly>& vector, CryptoContext<DCRTPoly> context) {
    uint32_t batchSize = vector->GetEncodingParameters()->GetBatchSize();

    matrix = resizeMatrix(matrix, batchSize, batchSize);
    auto diagonals = diagonal_transformation(matrix);

    unsigned int n1 = find_n1(batchSize);
    unsigned int n2 = batchSize / n1;

    Plaintext pl = context->MakeCKKSPackedPlaintext(diagonals[0]);
    Ciphertext<DCRTPoly> subResult = context->EvalMult(pl, vector);

    auto cipherPrecompute = context->EvalFastRotationPrecompute(vector);
    uint32_t M = 2 * context->GetRingDimension();

    std::vector<Ciphertext<DCRTPoly>> rotCache;
    std::vector<std::future<std::array<Ciphertext<DCRTPoly>, 2>>> futures1;

    for (unsigned int j=1; j<n1; j++) {
        futures1.push_back(std::async(
                std::launch::async,
                [&context, &diagonals, &cipherPrecompute, &vector, j, M] () -> std::array<Ciphertext<DCRTPoly>, 2> {
                    std::array<Ciphertext<DCRTPoly>, 2> result;
                    Plaintext pl = context->MakeCKKSPackedPlaintext(diagonals[j]);
                    Ciphertext<DCRTPoly> rotation = context->EvalFastRotation(vector, j, M, cipherPrecompute);
                    result[0] = rotation;
                    result[1] = context->EvalMult(pl, rotation);

                    return result;
                }
        ));
    }

    for (unsigned int j=0; j<n1-1; j++) {
        std::array<Ciphertext<DCRTPoly>, 2> buffer = futures1[j].get();

        rotCache.push_back(buffer[0]);
        subResult += buffer[1];
    }

    futures1.clear();

    Ciphertext<DCRTPoly> result = subResult;

    std::vector<std::future<Ciphertext<DCRTPoly>>> futures2;

    for (unsigned int k=1; k<n2; k++) {
        pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k*n1], -k*n1));
        subResult = context->EvalMult(pl, vector);

        futures2.push_back(std::async(
                std::launch::async,
            [&rotCache, &diagonals, &context, k, n1] (Ciphertext<DCRTPoly> subResult) -> Ciphertext<DCRTPoly> {
                for (unsigned int j = 1; j < n1; j++) {
                    Plaintext pl = context->MakeCKKSPackedPlaintext(rotate_plain(diagonals[k * n1 + j], -k * n1));
                    subResult += context->EvalMult(pl, rotCache[j - 1]);
                }

                return context->EvalRotate(subResult, k*n1);
            },
            subResult
        ));

    }

    for (unsigned int k=0; k<n2-1; k++) {
        result += futures2[k].get();
    }

    return result;
}


std::vector<double> plain_matrix_multiplication(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector) {
    std::vector<double> result;

    for (int i=0; i<(int) matrix.size(); i++) {
        double entry = 0;

        for (int j=0; j<(int) matrix[0].size(); j++) {
            entry += matrix[i][j] * vector[j];
        }

        result.push_back(entry);
    }

    return result;
}


std::vector<double> plain_addition(std::vector<double> a, const std::vector<double>& b) {
    for (int j=0; j<(int) a.size(); j++) {
        a[j] += b[j];
    }

    return a;
}

#include <iostream>
#include <iomanip>

#include "openfhe.h"
#include "LinTools.h"
#include "random_double.h"

using namespace lbcrypto;

int main() {
    int seed = time(NULL);
    srand((unsigned) seed);

    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    const uint32_t batchSize = 8;

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetBatchSize(batchSize);
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(50);

    auto context = GenCryptoContext(parameters);
    context->Enable(KEYSWITCH);
    context->Enable(PKE);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);

    KeyPair<DCRTPoly> keys = context->KeyGen();

    context->EvalMultKeyGen(keys.secretKey);

    std::vector<int> rotations(batchSize-1);
    for (uint32_t i=1; i<= batchSize; i++) {
        rotations[i-1] = (int) i;
    }
    context->EvalRotateKeyGen(keys.secretKey, rotations);
    std::cout << "Keygen done!\n\n";

    std::vector<double> plainText;
    for (uint32_t i=0; i<batchSize; i++) {
        plainText.push_back((double) i);
    }

    Plaintext pl = context->MakeCKKSPackedPlaintext(plainText);
    Ciphertext<DCRTPoly> ct = context->Encrypt(pl, keys.publicKey);

    std::cout << "matrix:" << std::endl;
    std::vector<std::vector<double>> matrix;
    for(int i=0; i < (int) batchSize/2; i++) {
        std::vector<double> row;
        for (int j=0; j<(int) batchSize; j++) {
            row.push_back((double) (random_double(0., 1.)));
        }
        matrix.push_back(row);
    }

    for (auto row: matrix) {
        std::cout << "[\t";
        for (auto number: row) {
            std::cout << number << "\t";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "diagonals: " << std::endl;
    auto diags = diagonal_transformation(matrix);
    for (auto row: diags) {
        std::cout << "[\t";
        for (auto number: row) {
            std::cout << number << "\t";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;

    std::vector<double> plainResult;
    for (int i=0; i<(int) matrix.size(); i++) {
        double entry = .0;
        for (int j=0; j<(int) matrix[0].size(); j++) {
            entry += matrix[i][j] * plainText[j];
        }
        plainResult.push_back(entry);
    }
    std::cout << "Expected result: [\t";
    for (auto entry: plainResult) {
        std::cout << entry << "\t";
    }
    std::cout << "]" << std::endl << std::endl;

    Ciphertext<DCRTPoly> cipherResult = matrix_multiplication(matrix, ct);
    Plaintext result;
    context->Decrypt(cipherResult, keys.secretKey, &result);
    result->SetLength(matrix[0].size());

    std::cout << result;

    return 0;
}

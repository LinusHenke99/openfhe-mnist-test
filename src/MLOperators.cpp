//
// Created by lchenke on 04.04.23.
//

#include "MLOperators.h"



Ciphertext<DCRTPoly> calculateMean(Ciphertext<DCRTPoly> input, CryptoContext<DCRTPoly> context, unsigned int N) {
    input = context->EvalSum(input, context->GetEncodingParams()->GetBatchSize());
    input = context->EvalMult(input, 1./N);

    return input;
}

Ciphertext<DCRTPoly> batchNorm(Ciphertext<DCRTPoly> input, CryptoContext<DCRTPoly> context, unsigned int polyDegree, double xMin, double xMax, unsigned int N) {
    auto mean = calculateMean(input, context, N);
}

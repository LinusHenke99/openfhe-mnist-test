//
// Created by lchenke on 04.04.23.
//

#include "MLOperators.h"



Ciphertext<DCRTPoly> calculateMean(Ciphertext<DCRTPoly> input, CryptoContext<DCRTPoly> context, unsigned int N) {
    input = context->EvalSum(input, context->GetEncodingParams()->GetBatchSize());
    input = context->EvalMult(input, (double) 1./N);

    return input;
}

Ciphertext<DCRTPoly> batchNorm(Ciphertext<DCRTPoly> input,
                               CryptoContext<DCRTPoly> context,
                               double mean,
                               double var,
                               double epsilon,
                               std::vector<double> gamma,
                               std::vector<double> beta) {

    Plaintext betas = context->MakeCKKSPackedPlaintext(beta);

    Plaintext gammas = context->MakeCKKSPackedPlaintext(gamma);

    Ciphertext<DCRTPoly> term = context->EvalMult(1/sqrt(var + epsilon), context->EvalAdd(input, -mean));

    Ciphertext<DCRTPoly> result = context->EvalAdd(context->EvalMult(term, gammas), betas);

    return result;
}

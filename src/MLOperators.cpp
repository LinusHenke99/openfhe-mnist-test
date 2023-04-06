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
                               unsigned int polyDegree,
                               double xMin,
                               double xMax,
                               unsigned int N,
                               double epsilon,
                               std::vector<double> gamma,
                               std::vector<double> beta) {

    auto mean = calculateMean(input, context, N);

    auto varianceAndEpsilon = context->EvalAdd(calculateMean(context->EvalMult(input, input), context, N) - context->EvalMult(mean, mean), epsilon);

    auto oneOverSquareRoot = [](double input) -> double {return 1. / sqrt(input);};

    auto denominator = context->EvalChebyshevFunction(oneOverSquareRoot, varianceAndEpsilon, xMin, xMax, polyDegree);

    auto numerator = input - mean;

    Plaintext betas = context->MakeCKKSPackedPlaintext(beta);

    Plaintext gammas = context->MakeCKKSPackedPlaintext(gamma);

    return context->EvalAdd(context->EvalMult(gammas, context->EvalMult(numerator, denominator)), betas);
}

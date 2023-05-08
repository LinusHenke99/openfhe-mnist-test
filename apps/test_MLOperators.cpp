#include <iostream>
#include <vector>
#include <iomanip>
#include <math.h>
#include <boost/range/combine.hpp>

#include "random_double.h"
#include "openfhe.h"
#include "MLOperators.h"

using namespace lbcrypto;


const double MIN = -1.;
const double MAX = 1.;

const uint32_t BATCHSIZE = 8;
const uint32_t MULT_DEPTH = 7;
const double EPSILON = 0.01;


int main() {
    int seed = time(NULL);
    srand((unsigned) seed);

    std::vector<double> input;
    std::vector<double> betas, gammas;

    for (unsigned int _i = 0; _i<BATCHSIZE; _i++) {
        input.push_back(random_double(MIN, MAX));
        gammas.push_back(random_double(MIN, MAX));
        betas.push_back(random_double(MIN, MAX));
    }

    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetBatchSize(BATCHSIZE);
    parameters.SetMultiplicativeDepth(MULT_DEPTH);
    parameters.SetFirstModSize(40);
    parameters.SetScalingModSize(35);
    parameters.SetScalingTechnique(FIXEDAUTO);

    auto context = GenCryptoContext(parameters);
    context->Enable(KEYSWITCH);
    context->Enable(PKE);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);

    auto keys = context->KeyGen();
    std::cout << "Keypair generated" << std::endl;

    std::cout << "Generating evaluation keys...";
    context->EvalMultKeyGen(keys.secretKey);
    context->EvalSumKeyGen(keys.secretKey);

    std::cout << "...done!" << std::endl;

    std::cout << "Sample vector: [\t";

    double plainMean = 0.;
    double meanOfSquares = 0.;
    for (auto elem : input) {
        std::cout << elem << "\t";
        plainMean += elem;
        meanOfSquares += elem * elem;
    }
    plainMean /= BATCHSIZE;
    meanOfSquares /= BATCHSIZE;

    std::vector<double> plainBatchNorm;

    for (auto tup : boost::combine(input, betas, gammas)) {
        double x, beta, gamma;
        boost::tie(x, beta, gamma) = tup;
        plainBatchNorm.push_back(gamma * (x - plainMean) / sqrt(meanOfSquares - plainMean + EPSILON) + beta);
    }

    double plainVar = meanOfSquares - plainMean;
    std::cout << "]" << std::endl << "Plain mean:         \t " << plainMean << std::endl;
    std::cout << "Plain variance: " << plainVar << std::endl;
    std::cout << "Plain batch noramlized input: [\t";
    for (auto elem : plainBatchNorm)
        std::cout << elem << "\t";
    std::cout << "]" << std::endl << std::endl;

    Plaintext pl = context->MakeCKKSPackedPlaintext(input);

    auto cipherInput = context->Encrypt(pl, keys.publicKey);

    auto cipherMean = calculateMean(cipherInput, context, BATCHSIZE);
    auto cipherVariance = context->EvalSub(calculateMean(context->EvalMult(cipherInput, cipherInput), context, BATCHSIZE), cipherMean);
    // auto cipherBatchNorm = batchNorm(cipherInput, context, 3, plainVar + EPSILON, plainVar + EPSILON, BATCHSIZE, .0, gammas, betas);

    std::cout << "Ciphertext Level: " << cipherBatchNorm->GetLevel() << std::endl;

    context->Decrypt(cipherMean, keys.secretKey, &pl);
    pl->SetLength(1);

    std::cout << "Ciphertext mean: " << pl;

    context->Decrypt(cipherVariance, keys.secretKey, &pl);
    pl->SetLength(1);
    std::cout << "Ciphertext variance: " << pl;


    context->Decrypt(cipherBatchNorm, keys.secretKey, &pl);
    pl->SetLength(BATCHSIZE);

    std::cout << "Ciphertext batch normalization: " << pl;

    return 0;
}
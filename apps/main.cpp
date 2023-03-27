#include <iostream>
#include <chrono>
#include <iomanip>
#include <future>
typedef std::chrono::high_resolution_clock Clock;


#include "Deserialize.h"
#include "openfhe.h"
#include "LinTools.h"
#include "LoadModel.h"
#include "LoadImage.h"


using namespace lbcrypto;


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax);


double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
                            std::chrono::time_point<std::chrono::system_clock> end);


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Give file as argument." << std::endl;
        return 0;
    }

    std::cout << std::setprecision(3);

    auto start = Clock::now();

    std::string filename = argv[1];

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> cipher;

    //  Deserializing the context that was generated by the keygen program
    auto desStart = Clock::now();
    DeserializeContext(context, keys);
    DeserializeCiphertext(cipher, filename);
    auto desEnd = Clock::now();

    std::cout << "Loading model...";

    //  Loading the convolutional layer weights and biases
    auto convWeights = loadMatrix("../model/_Conv_0_weights.csv");
    convWeights = transpose(convWeights);
    std::vector<double> convBiasesVec = loadBias("../model/_Conv_0_bias.csv");
    Plaintext convBiases = context->MakeCKKSPackedPlaintext(convBiasesVec);

    //  Loading the gemm3 weights and biases
    auto gemm1Weights = loadMatrix("../model/_Gemm_3_w.csv");
    gemm1Weights = transpose(gemm1Weights);
    std::vector<double> gemm1BiasesVec = loadBias("../model/_Gemm_3_bias.csv");
    Plaintext gemm1Biases = context->MakeCKKSPackedPlaintext(gemm1BiasesVec);

    // Loading the gemm5 weights and biases
    auto gemm2Weights = loadMatrix("../model/_Gemm_5_w.csv");
    gemm2Weights = transpose(gemm2Weights);
    std::vector<double> gemm2BiasesVec = loadBias("../model/_Gemm_5_bias.csv");
    Plaintext gemm2Biases = context->MakeCKKSPackedPlaintext(gemm2BiasesVec);
    std::cout << "Done!" << std::endl;


    /*
     *      Starting with Cryptonet operations
     */

    //  Applying convolutional layer
    std::cout << "conv...";
    auto startOper = Clock::now();
    cipher = matrix_multiplication(convWeights, cipher, context);
    cipher = context->EvalAdd(cipher, convBiases);
    auto endOper = Clock::now();
    std::cout << "Done! Execution time: " << time_in_seconds(startOper, endOper) << std::endl;


    // Applying first instance of relu
    std::cout << "relu1...";
    double xMin = -6.5318193435668945;
    double xMax = 8.548895835876465;

    startOper = Clock::now();
    cipher = evalRelu(cipher, xMin, xMax);
    endOper = Clock::now();

    std::cout << "Done! Execution time: " << time_in_seconds(startOper, endOper) << std::endl;


    // Applying first fully connected layer
    std::cout << "gemm3...";
    startOper = Clock::now();
    cipher = matrix_multiplication(gemm1Weights, cipher, context);
    cipher = context->EvalAdd(cipher, gemm1Biases);
    endOper = Clock::now();
    std::cout << "Done! Execution time: " << time_in_seconds(startOper, endOper) << std::endl;


    // Applying second instance of relu
    std::cout << "relu2...";
    xMin = -14.685586750507355;
    xMax = 12.968225657939911;

    startOper = Clock::now();
    cipher = evalRelu(cipher, xMin, xMax);
    endOper = Clock::now();

    std::cout << "Done! Execution time: " << time_in_seconds(startOper, endOper) << std::endl;


    // Applying second fully connected layer
    std::cout << "gemm5...";
    startOper = Clock::now();
    cipher = matrix_multiplication(gemm2Weights, cipher, context);
    cipher = context->EvalAdd(cipher, gemm2Biases);
    endOper = Clock::now();
    std::cout << "Done! Execution time: " << time_in_seconds(startOper, endOper) << std::endl;

    auto end = Clock::now();

    double execTime = time_in_seconds(start, end);

    Plaintext result;
    context->Decrypt(keys.secretKey, cipher, &result);
    result->SetLength(10);
    auto resultVec = result->GetRealPackedValue();

    auto it = std::minmax_element(resultVec.begin(), resultVec.end());
    int res = std::distance(resultVec.begin(), it.second);

    std::cout << "The number is a " << res << ", the input file was named " << filename << std::endl;

    double desTime = time_in_seconds(desStart, desEnd);
    std::cout << "Took " << execTime << " seconds, where " << desTime << "s were spent deserializing, and "
            << execTime - desTime << "s were spent doing ML operations." << std::endl;

    std::cout << "The output of the neurons was: " << result << std::endl;

    return 0;
}


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax) {
    auto context = cipher->GetCryptoContext();

    return context->EvalChebyshevFunction([](double x) -> double {return x < .0 ? 0 : x;}, cipher, xMin, xMax, 3);
}


double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
                            std::chrono::time_point<std::chrono::system_clock> end) {
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1E-9;
}

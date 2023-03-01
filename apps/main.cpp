#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include "Deserialize.h"
#include "openfhe.h"
#include "LinTools.h"
#include "LoadModel.h"
#include "LoadImage.h"

using namespace lbcrypto;


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax);

void myReplace(std::string& str,
               const std::string& oldStr,
               const std::string& newStr);


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Give file as argument." << std::endl;
        return 0;
    }

    auto start = Clock::now();

    std::string filename = argv[1];

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> cipher;

    DeserializeContext(context, keys);
    DeserializeCiphertext(cipher, filename);

    std::cout << "Loading model...";
    auto convWeights = loadMatrix("../model/_Conv_0_weights.csv");
    convWeights = transpose(convWeights);
    std::vector<double> convBiasesVec = loadBias("../model/_Conv_0_bias.csv");
    Plaintext convBiases = context->MakeCKKSPackedPlaintext(convBiasesVec);

    auto gemm1Weights = loadMatrix("../model/_Gemm_3_w.csv");
    gemm1Weights = transpose(gemm1Weights);
    std::vector<double> gemm1BiasesVec = loadBias("../model/_Gemm_3_bias.csv");
    Plaintext gemm1Biases = context->MakeCKKSPackedPlaintext(gemm1BiasesVec);

    auto gemm2Weights = loadMatrix("../model/_Gemm_5_w.csv");
    gemm2Weights = transpose(gemm2Weights);
    std::vector<double> gemm2BiasesVec = loadBias("../model/_Gemm_5_bias.csv");
    Plaintext gemm2Biases = context->MakeCKKSPackedPlaintext(gemm2BiasesVec);
    std::cout << "Done!" << std::endl;

    /*
     *      Carrying out convolution layer
     */
    std::cout << "conv...";
    cipher = matrix_multiplication_diagonals(convWeights, cipher);
    cipher = context->EvalAdd(cipher, convBiases);
    std::cout << "Done!" << std::endl;

    /*
     *      Evaluate Relu function
     */
    std::cout << "relu1...";
    double xMin = -6.5318193435668945;
    double xMax = 8.548895835876465;

    cipher = evalRelu(cipher, xMin, xMax);
    std::cout << "Done!" << std::endl;

    /*
     *      Evaluate first fully connected layer
     */
    std::cout << "gemm3...";
    cipher = matrix_multiplication_diagonals(gemm1Weights, cipher);
    cipher = context->EvalAdd(cipher, gemm1Biases);
    std::cout << "Done!" << std::endl;

    /*
     *      Evaluate second instance of Relu function
     */
    std::cout << "relu2...";
    xMin = -14.685586750507355;
    xMax = 12.968225657939911;

    cipher = evalRelu(cipher, xMin, xMax);
    std::cout << "Done!" << std::endl;

    /*
     *      Evaluate last fully connected layer
     */
    std::cout << "gemm5...";
    cipher = matrix_multiplication_diagonals(gemm2Weights, cipher);
    cipher = context->EvalAdd(cipher, gemm2Biases);
    std::cout << "Done!" << std::endl;

    auto end = Clock::now();

    double execTime = (double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1E-9;

    Plaintext result;
    context->Decrypt(keys.secretKey, cipher, &result);
    result->SetLength(10);
    auto resultVec = result->GetRealPackedValue();

    auto it = std::minmax_element(resultVec.begin(), resultVec.end());
    int res = std::distance(resultVec.begin(), it.second);

    std::cout << "The number is a: " << res << std::endl;

    std::cout << "Took " << execTime << " seconds" << std::endl;

    std::cout << "[\t";
    for(auto elem : resultVec) {
        std::cout << elem << "\t";
    }
    std::cout << "]\n";

    myReplace(filename, "../ciphertexts/", "../img/");
    myReplace(filename, ".txt", ".csv");
    std::cout << filename << std::endl;
    std::vector<double> vec = load_image(filename);
    vec.push_back(.0);

    std::cout << "Plain convolution" << std::endl;
    //      Plain convolution
    std::cout << vec.size() << std::endl;
    std::cout << convWeights[0].size() << std::endl;
    vec = plain_matrix_multiplication(convWeights, vec);
    vec = plain_addition(vec, convBiasesVec);

    std::cout << "Plain relu1" << std::endl;
    //      Plain relu
    for (auto& elem : vec)
        elem = elem < .0 ? .0 : elem;

    //      Plain gemm1
    std::cout << "Plain gemm1" << std::endl;
    vec = plain_matrix_multiplication(gemm1Weights, vec);
    vec = plain_addition(vec, gemm1BiasesVec);

    //      Plain relu
    std::cout << "Plain relu2" << std::endl;
    for (auto& elem : vec)
        elem = elem < .0 ? .0 : elem;

    //      Plain gemm2
    std::cout << "Plain gemm2" << std::endl;
    vec = plain_matrix_multiplication(gemm2Weights, vec);
    vec = plain_addition(vec, gemm2BiasesVec);

    std::cout << "[\t";
    for(auto elem : vec) {
        std::cout << elem << "\t";
    }
    std::cout << "]\n";

    return 0;
}


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax) {
    auto context = cipher->GetCryptoContext();

    return context->EvalChebyshevFunction([](double x) -> double {return x < .0 ? 0 : x;}, cipher, xMin, xMax, 4);
}


void myReplace(std::string& str,
               const std::string& oldStr,
               const std::string& newStr)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(oldStr, pos)) != std::string::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

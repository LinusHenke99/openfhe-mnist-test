#include <iostream>
#include <vector>
#include <string>

#include "openfhe.h"
#include "LinTools.h"
#include "Deserialize.h"
#include "LoadModel.h"

using namespace lbcrypto;

const std::string MODEL_PATH = "../model/very_sensible_nn/";


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax);


// double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
//                         std::chrono::time_point<std::chrono::system_clock> end);


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Give file as argument." << std::endl;
        exit(0);
    }

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;

    Ciphertext<DCRTPoly> cipherInput;
    std::string filename = argv[1];

    DeserializeContext(context, keys, very_sensible_nn);
    DeserializeCiphertext(cipherInput, filename, very_sensible_nn);


    //  Min and Max values for ReLU after first convolutional layer
    // double xMin = -0.23868335783481598, xMax = 0.5498048663139343;

    //  Min and Max values for ReLU after first linear layer
    // xMin = -0.06918778270483017, xMax = 0.06864853203296661;

    //  Min and Max values for ReLU after second linear layer
    // xMin = -0.05904359742999077, xMax = 0.06073193624615669;

    //  Min and Max values for ReLU after third linear layer
    // xMin = -0.06039716675877571, xMax = 0.06716714799404144;

    return 0;
}



Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax) {
    auto context = cipher->GetCryptoContext();

    return context->EvalChebyshevFunction([](double x) -> double {return x < .0 ? 0 : x;}, cipher, xMin, xMax, 3);
}


// double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
//                         std::chrono::time_point<std::chrono::system_clock> end) {
//     return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1E-9;
// }

#include <iostream>
#include <string>
#include <vector>

#include "openfhe.h"
#include "openfhecore.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"

using namespace lbcrypto;

const std::string FOLDER = "../keys/";

int main(int argc, char** argv) {
    uint32_t multDepth = std::stoi(argv[1]);
    uint32_t scalSize = std::stoi(argv[2]);
    const uint32_t batchSize = 32*32;

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scalSize);
    parameters.SetSecurityLevel(HEStd_128_classic);
    parameters.SetBatchSize(batchSize);

    CryptoContext<DCRTPoly> context = GenCryptoContext(parameters);

    std::cout << "The Cryptocontext has been generated..." << std::endl;

    if(!Serial::SerializeToFile(FOLDER + "context.txt", context, SerType::BINARY)) {
        std::cerr << "Error writing serialization of context." << std::endl;
        return 1;
    }

    return 0;
}

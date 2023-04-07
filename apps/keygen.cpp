#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <math.h>

#include "KeyGen.h"

#include "openfhe.h"

#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"

using namespace lbcrypto;

const std::string FOLDER = "../keys/";

const uint32_t batchSize = 1024;
const std::vector<uint32_t> LEVEL_BUDGET = {2, 2};
const std::vector<uint32_t> BSGS_DIM = {0, 0};
const auto SECRET_KEY_DIST = UNIFORM_TERNARY;


int main (int argc, char** argv) {
    bool bootstrapping = false;

    if (argc != 8 && argc != 7) {
        std::cout << "args: <model> <mult depth> <mod size> <first mod size> <security level> <ringdim> if you don't "
                     "want to use bootstrapping" << std::endl;
        std::cout << "args: <model> <approx bootstrap depth> <mod size> <first mod size> <security level> <ringdim> "
                     "<levels used before bootstrapping> if you want to use bootstrapping" << std::endl;
        std::cout << argc << std::endl;
        return 0;
    } else if (argc == 8) {
        bootstrapping = true;
    }


    std::string model = argv[1];
    uint32_t scalSize = std::stoi(argv[3]);
    uint32_t firstModSize = std::stoi(argv[4]);
    uint32_t securityLevel = std::stoi(argv[5]);

    std::string subFolder = "";

    if (model == "cryptonet" || model == "very_sensible_nn") {
        subFolder = model + "/";
    }

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetRingDim(next_power2(std::stoi(argv[6])));
    parameters.SetScalingModSize(scalSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);

    switch (securityLevel) {
        case 128:
            parameters.SetSecurityLevel(HEStd_128_classic);
            break;

        case 256:
            parameters.SetSecurityLevel(HEStd_256_classic);
            break;

        case 192:
            parameters.SetSecurityLevel(HEStd_192_classic);
            break;

        default:
            parameters.SetSecurityLevel(HEStd_NotSet);
    }

    if (bootstrapping) {
        uint32_t levelsUsedBeforeBootstrapping = std::stoi(argv[7]);
        uint32_t approxBootDepth = std::stoi(argv[2]);

        parameters.SetSecretKeyDist(SECRET_KEY_DIST);

        usint depth =
                levelsUsedBeforeBootstrapping +
                FHECKKSRNS::GetBootstrapDepth(approxBootDepth, LEVEL_BUDGET, SECRET_KEY_DIST);

        parameters.SetMultiplicativeDepth(depth);
    } else {
        uint32_t multDepth = std::stoi(argv[2]);

        parameters.SetMultiplicativeDepth(multDepth);
    }


    CryptoContext<DCRTPoly> context = GenCryptoContext(parameters);

    context->Enable(PKE);
    context->Enable(KEYSWITCH);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);

    std::cout << "The Cryptocontext has been generated..." << std::endl;
    std::cout << "q: " << context->GetModulus() << std::endl;
    std::cout << "Ring dimension: " << context->GetRingDimension() << std::endl;

    if (bootstrapping) {
        context->Enable(FHE);
        std::cout << "Starting with bootstrapping setup...";
        context->EvalBootstrapSetup(LEVEL_BUDGET, BSGS_DIM, batchSize);
        std::cout << "Done!" << std::endl;
    }

    KeyPair<DCRTPoly> keys = context->KeyGen();
    std::cout << "Keypair has been generated..." << std::endl;

    context->EvalMultKeyGen(keys.secretKey);
    std::cout << "Eval. mult. keys generated" << std::endl;

    std::vector<int> rotations = genRotations(batchSize);

    std::cout << "Generating rotation keys..." << std::endl;
    context->EvalRotateKeyGen(keys.secretKey, rotations);
    std::cout << "Done!" << std::endl;

    if (bootstrapping) {
        std::cout << "Generating bootstrapping keys...";
        context->EvalBootstrapKeyGen(keys.secretKey, batchSize);
        std::cout << "Done!" << std::endl;
    }



    if (!Serial::SerializeToFile(FOLDER + subFolder + "context.txt", context, SerType::BINARY)) {
        std::cerr << "Error serializing context." << std::endl;
        std::exit(1);
    }
    std::cout << "Cryptocontext serialized!" << std::endl;

    if (!Serial::SerializeToFile(FOLDER + subFolder + "publicKey.txt", keys.publicKey, SerType::BINARY)) {
        std::cerr << "Error serializing public key." << std::endl;
        std::exit(1);
    }
    std::cout << "Public key serialized!" << std::endl;

    if (!Serial::SerializeToFile(FOLDER + subFolder + "secretKey.txt", keys.secretKey, SerType::BINARY)) {
        std::cerr << "Error serializing public key." << std::endl;
        std::exit(1);
    }
    std::cout << "Secret key serialized!" << std::endl;

    std::ofstream multKeyFile(FOLDER + subFolder + "multKey.txt", std::ios::out | std::ios::binary);
    if (multKeyFile.is_open()) {
        if (!context->SerializeEvalMultKey(multKeyFile, SerType::BINARY)) {
            std::cerr << "Error serializing multiplication key." << std::endl;
            std::exit(1);
        }
        std::cout << "Multiplication key serialized!" << std::endl;
        multKeyFile.close();

    } else {
        std::cerr << "Error opening Mult Key file..." << std::endl;
    }

    std::ofstream rotKeyFile(FOLDER + subFolder + "rotKey.txt", std::ios::out | std::ios::binary);
    if (rotKeyFile.is_open()) {
        if (!context->SerializeEvalAutomorphismKey(rotKeyFile, SerType::BINARY)) {
            std::cerr << "Error serializing rotation key." << std::endl;
            std::exit(1);
        }
        std::cout << "Rotation key serialized!" << std::endl;
        rotKeyFile.close();
    } else {
        std::cerr << "Error opening Mult Key file..." << std::endl;
    }

    return 0;
}

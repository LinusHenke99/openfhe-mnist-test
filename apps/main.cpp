#include <iostream>

#include "LoadImage.h"
#include "openfhe.h"

using namespace lbcrypto;

int main(int argc, char** argv) {
    uint32_t batchSize = 32*32;
    uint32_t multDepth = 6;
    uint32_t scalSize = 50;
    const std::vector<double> polyParams = {1., 2., 3.};

    std::string filePath = "../img/test.file";

    std::vector<double> img = load_image(filePath);

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetSecurityLevel(HEStd_128_classic);
    parameters.SetBatchSize(batchSize);
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scalSize);

    CryptoContext<DCRTPoly> context = GenCryptoContext(parameters);
    context->Enable(PKE);
    context->Enable(KEYSWITCH);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);

    auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);

    std::vector<int> rotations(batchSize/2 -1);
    for(uint32_t i=1; i<batchSize/2; i++) {
        rotations[i] = i;
    }
    context->EvalRotateKeyGen(keys.secretKey, rotations);

    std::cout << "Ring dimension: " << context->GetRingDimension() << std::endl;

    Plaintext pl = context->MakeCKKSPackedPlaintext(img);
    Ciphertext<DCRTPoly> cipher = context->Encrypt(keys.publicKey, pl);

    return 0;
}

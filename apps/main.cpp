#include <iostream>

#include "Deserialize.h"
#include "LoadImage.h"
#include "openfhe.h"
#include "LinTools.h"

using namespace lbcrypto;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Give file as argument." << std::endl;
        return 0;
    }

    std::string filename = argv[1];

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> cipher;

    DeserializeContext(context, keys);
    DeserializeCiphertext(cipher, filename);



    return 0;
}

#include <iostream>
#include <vector>
#include <string>

#include "LoadImage.h"
#include "Deserialize.h"
#include "openfhe.h"
#include "ciphertext-ser.h"

using namespace lbcrypto;

const std::string FOLDER = "../ciphertexts/";

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Give filename as argument." << std::endl;
        return 0;
    }

    std::string fileName = argv[1];

    std::vector<double> img = load_image(fileName);
    int size = sqrt(img.size());

    std::cout << "Image: " << std::endl;
    for(int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            char integer = img[i*28 + j] == 0 ? '-' : '+';
            std::cout << integer << " ";
        }
        std::cout << std::endl;
    }

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;


    std::cout << std::endl;
    DeserializeContext(context, keys, false);

    Plaintext IMG = context->MakeCKKSPackedPlaintext(img);
    if(!IMG->Encode()) {
        std::cerr << "Couldn't encode Plaintext." << std::endl;
    }
    std::cout << "Image encoded." << std::endl;

    Ciphertext<DCRTPoly> IMG_cipher = context->Encrypt(keys.publicKey, IMG);
    std::cout << "Ciphertext encrypted." << std::endl;

    if(!Serial::SerializeToFile(FOLDER + fileName[fileName.size() - 5] + ".txt", IMG_cipher, SerType::BINARY)) {
        std::cerr << "Error Serializing ciphertext." << std::endl;
        exit(1);
    }
    std::cout << "Ciphertext serialized." << std::endl;

    return 0;
}

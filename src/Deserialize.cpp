#include "Deserialize.h"

std::string FOLDER = "../keys/";
std::string CIPHER_FOLDER = "../ciphertexts/";


std::string setSubFolder (Model model) {
    std::string subFolder;

    switch (model) {
        case cryptonet:
            subFolder = "cryptonet/";
            break;

        case very_sensible_nn:
            subFolder = "very_sensible_nn/";
            break;

        default:
            subFolder = "";
            break;
    }

    return subFolder;
}


void DeserializeContext(CryptoContext<DCRTPoly>& context, KeyPair<DCRTPoly>& keyPair, Model model, bool evalKeys) {
    context->ClearEvalMultKeys();
    context->ClearEvalAutomorphismKeys();
    lbcrypto::CryptoContextFactory<lbcrypto::DCRTPoly>::ReleaseAllContexts();

    std::string subFolder = setSubFolder(model);


    if (!Serial::DeserializeFromFile(FOLDER + subFolder + "context.txt", context, SerType::BINARY)) {
        std::cerr << "Error deserializing context!" << std::endl;
        std::exit(1);
    }
    std::cout << "Context deserialized!" << std::endl;

    if (!Serial::DeserializeFromFile(FOLDER + subFolder + "publicKey.txt", keyPair.publicKey, SerType::BINARY)) {
        std::cerr << "Error deserializing public key!" << std::endl;
        std::exit(1);
    }
    std::cout << "Public key deserialized!" << std::endl;

    if (!Serial::DeserializeFromFile(FOLDER + subFolder + "secretKey.txt", keyPair.secretKey, SerType::BINARY)) {
        std::cerr << "Error reading serializing secret key!" << std::endl;
        std::exit(1);
    }
    std::cout << "Private key deserialized!" << std::endl;

    if (evalKeys) {
        std::ifstream multKeyIStream(FOLDER + subFolder + "multKey.txt", std::ios::in | std::ios::binary);
        if (!multKeyIStream.is_open()) {
            std::cerr << "Error opening mult. key file." << std::endl;
            std::exit(1);
        }
        if (!context->DeserializeEvalMultKey(multKeyIStream, SerType::BINARY)) {
            std::cerr << "Error deserializing mult. key." << std::endl;
            std::exit(1);
        }
        std::cout << "Deserialized mult. key" << std::endl;
        multKeyIStream.close();

        std::ifstream rotKeyIStream(FOLDER + subFolder + "rotKey.txt", std::ios::in | std::ios::binary);
        if (!rotKeyIStream.is_open()) {
            std::cerr << "Error opening rot. key file." << std::endl;
            std::exit(1);
        }
        if (!context->DeserializeEvalAutomorphismKey(rotKeyIStream, SerType::BINARY)) {
            std::cerr << "Error deserializing rot. key." << std::endl;
            std::exit(1);
        }
        std::cout << "Deserialized rot. key" << std::endl;
        rotKeyIStream.close();
    }
}


void DeserializeCiphertext(Ciphertext<DCRTPoly>& cipher, std::string filename, Model model) {
    std::string subFolder = setSubFolder(model);

    if (!Serial::DeserializeFromFile(CIPHER_FOLDER + subFolder + filename, cipher, SerType::BINARY)) {
        std::cerr << "Could not deserialize " + filename + " ciphertext" << std::endl;
        exit(1);
    }
    std::cout << "Ciphertext " + filename << " deserialized." << std::endl;
}

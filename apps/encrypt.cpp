#include <iostream>
#include <vector>
#include <string>

#include "LoadImage.h"
#include "Deserialize.h"
#include "openfhe.h"
#include "ciphertext-ser.h"

using namespace lbcrypto;

const std::string FOLDER = "../ciphertexts/";


void myReplace(std::string& str,
               const std::string& oldStr,
               const std::string& newStr);


int main(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "args: <model> <file>" << std::endl;
        return 0;
    }

    std::string fileName = argv[2];

    std::vector<double> img = load_image(fileName);
    int size = sqrt(img.size());
    std::cout << size << std::endl;

    std::cout << "Image: " << std::endl;
    for(int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            char integer = img[i*size + j] == 0 ? '.' : '@';
            std::cout << integer << " ";
        }
        std::cout << std::endl;
    }

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;

    std::string modelStr = argv[1];
    Model model;
    if (modelStr == "cryptonet") {
        model = cryptonet;
    }else if (modelStr == "very_sensible_nn") {
        model = very_sensible_nn;
    }else {
        model = none;
    }

    std::string subFolder = setSubFolder(model);

    std::cout << std::endl;
    DeserializeContext(context, keys, model, false);

    Plaintext IMG = context->MakeCKKSPackedPlaintext(img);
    if(!IMG->Encode()) {
        std::cerr << "Couldn't encode Plaintext." << std::endl;
    }
    std::cout << "Image encoded." << std::endl;

    Ciphertext<DCRTPoly> IMG_cipher = context->Encrypt(keys.publicKey, IMG);
    std::cout << "Ciphertext encrypted." << std::endl;

    myReplace(fileName, ".csv", ".txt");
    myReplace(fileName, "../img/" + modelStr + "/", "");

    std::cout << fileName << std::endl;

    if(!Serial::SerializeToFile(FOLDER + subFolder + fileName, IMG_cipher, SerType::BINARY)) {
        std::cerr << "Error Serializing ciphertext." << std::endl;
        exit(1);
    }
    std::cout << "Ciphertext serialized." << std::endl;

    return 0;
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

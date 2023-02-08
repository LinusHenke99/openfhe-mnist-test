#ifndef TEST_MNIST_DESERIALIZE_H
#define TEST_MNIST_DESERIALIZE_H

#include <iostream>

#include "openfhe.h"

#include "key/key-ser.h"
#include "cryptocontext-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"

using namespace lbcrypto;

void DeserializeContext(CryptoContext<DCRTPoly>& context, KeyPair<DCRTPoly>& keypair, bool evalKeys = true);

void DeserializeCiphertext(Ciphertext<DCRTPoly>& cipher, std::string filename);

#endif //TEST_MNIST_DESERIALIZE_H

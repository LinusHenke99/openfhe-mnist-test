#include <iostream>
#include <vector>
#include <string>

#include "openfhe.h"
#include "LinTools.h"
#include "Deserialize.h"
#include "LoadModel.h"
#include "LoadImage.h"

using namespace lbcrypto;

const std::string MODEL_PATH = "../model/very_sensible_nn/";
const std::string PATH_TO_IMAGE = "../img/very_sensible_nn/";
const std::vector<uint32_t> LEVEL_BUDGET = {3, 3};
const std::vector<uint32_t> BSGS_DIM = {0, 0};


Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax);


// double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
//                         std::chrono::time_point<std::chrono::system_clock> end);


void myReplace(std::string& str,
               const std::string& oldStr,
               const std::string& newStr);

void print_shape(std::vector<std::vector<double>> matrix);


void plainReLU (std::vector<double>& input);


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Give file as argument." << std::endl;
        exit(0);
    }

    CryptoContext<DCRTPoly> context;
    KeyPair<DCRTPoly> keys;

    Ciphertext<DCRTPoly> cipherInput;
    std::string filename = argv[1];

    std::string image_file = filename;
    myReplace(image_file, ".txt", ".csv");
    std::cout << image_file << std::endl;
	std::cout << PATH_TO_IMAGE + image_file << std::endl;
    std::vector<double> plainInput = load_image(PATH_TO_IMAGE + image_file);

    DeserializeContext(context, keys, very_sensible_nn);
    DeserializeCiphertext(cipherInput, filename, very_sensible_nn);

	// context->EvalBootstrapSetup(LEVEL_BUDGET, BSGS_DIM, 1024);


    std::cout << "Loading model variables..." << std::endl;
    std::cout << "Loading first convolutional layer." << std::endl;
    auto conv0_weights = loadMatrix(MODEL_PATH +  "_conv.0_weights.csv");
	conv0_weights = transpose(conv0_weights);
    auto conv0_bias = loadBias(MODEL_PATH + "_conv.0_bias.csv");

	print_shape(conv0_weights);
	std::cout << "biases: " << conv0_bias.size() << std::endl;


    std::cout << "Loading second convolutional layer." << std::endl;
    auto conv1_weights = loadMatrix(MODEL_PATH +  "_conv.2_weights.csv");
	conv1_weights = transpose(conv1_weights);
	auto conv1_bias = loadBias(MODEL_PATH + "_conv.2_bias.csv");

	print_shape(conv1_weights);
	std::cout << "biases: " << conv1_bias.size() << std::endl;


    std::cout << "Loading first linear layer." << std::endl;
    auto lin0_weights = loadMatrix(MODEL_PATH + "_lin.0_weights.csv");
	lin0_weights = transpose(lin0_weights);
    auto lin0_bias = loadBias(MODEL_PATH + "_lin.0_bias.csv");

	print_shape(lin0_weights);
	std::cout << "biases: " << lin0_bias.size() << std::endl;


    std::cout << "Loading second linear layer." << std::endl;
    auto lin1_weights = loadMatrix(MODEL_PATH + "_lin.2_weights.csv");
	lin1_weights = transpose(lin1_weights);
    auto lin1_bias = loadBias(MODEL_PATH + "_lin.2_bias.csv");

	print_shape(lin1_weights);
	std::cout << "biases: " << lin1_bias.size() << std::endl;


    std::cout << "Loading third linear layer." << std::endl;
    auto lin2_weights = loadMatrix(MODEL_PATH + "_lin.4_weights.csv");
	lin2_weights = transpose(lin2_weights);
    auto lin2_bias = loadBias(MODEL_PATH + "_lin.4_bias.csv");

	print_shape(lin2_weights);
	std::cout << "biases: " << lin2_bias.size() << std::endl;


    std::cout << "Loading fourth linear layer." << std::endl;
    auto lin3_weights = loadMatrix(MODEL_PATH + "_lin.6_weights.csv");
	lin3_weights = transpose(lin3_weights);
    auto lin3_bias = loadBias(MODEL_PATH + "_lin.6_bias.csv");
	
	print_shape(lin3_weights);
	std::cout << "biases: " << lin3_bias.size() << std::endl;

    std::cout << "Done!" << std::endl;




    //  Executing first convolutional layer
	std::cout << "Executing first convolutional layer...";
    cipherInput = matrix_multiplication(conv0_weights, cipherInput, context);
    Plaintext pl = context->MakeCKKSPackedPlaintext(conv0_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
	std::cout << "Done!" << std::endl;

	plainInput = plain_matrix_multiplication(conv0_weights, plainInput);
    plainInput = plain_addition(plainInput, conv0_bias);

	std::cout << "Output after first convolutional layer." << std::endl;

	context->Decrypt(cipherInput, keys.secretKey, &pl);
	pl->SetLength(529);
	std::cout << pl;

	// std::cout << "Plain result:" << std::endl;
	// std::cout << "[\t";
	// for (const auto& entry : plainInput)
	// 	std::cout << entry << "\t";
	// std::cout << "]" << std::endl;


    //  Min and Max values for ReLU after first convolutional layer
	std::cout << "First Relu...";
    double xMin = -0.23868335783481598, xMax = 0.5498048663139343;
    cipherInput = evalRelu(cipherInput, xMin, xMax);
	std::cout << "Done!" << std::endl;

    plainReLU(plainInput);


    //  Executing second convolutional layer
    std::cout << "Executing second convolutional layer...";
    cipherInput = matrix_multiplication(conv1_weights, cipherInput, context);
    pl = context->MakeCKKSPackedPlaintext(conv1_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
    std::cout << "Done!" << std::endl;


    plainInput = plain_matrix_multiplication(conv1_weights, plainInput);
    plainInput = plain_addition(plainInput, conv1_bias);


    //  Executing first linear layer
    std::cout << "Executing first linear layer...";
    cipherInput = matrix_multiplication(lin0_weights, cipherInput, context);
    pl = context->MakeCKKSPackedPlaintext(lin0_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
    std::cout << "Done!" << std::endl;

    plainInput = plain_matrix_multiplication(lin0_weights, plainInput);
    plainInput = plain_addition(plainInput, lin0_bias);


    //  Min and Max values for ReLU after first linear layer
    std::cout << "Second Relu...";
    xMin = -0.06918778270483017, xMax = 0.06864853203296661;
    cipherInput = evalRelu(cipherInput, xMin, xMax);
    std::cout << "Done!" << std::endl;

    // cipherInput = context->EvalBootstrap(cipherInput);

    plainReLU(plainInput);


    //  Executing second linear layer
    std::cout << "Executing second linear layer...";
    cipherInput = matrix_multiplication(lin1_weights, cipherInput, context);
    pl = context->MakeCKKSPackedPlaintext(lin1_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
    std::cout << "Done!" << std::endl;

    plainInput = plain_matrix_multiplication(lin1_weights, plainInput);
    plainInput = plain_addition(lin1_bias, plainInput);

    //  Min and Max values for ReLU after second linear layer
    std::cout << "Third Relu...";
    xMin = -0.05904359742999077, xMax = 0.06073193624615669;
    cipherInput = evalRelu(cipherInput, xMin, xMax);
    std::cout << "Done!" << std::endl;

    // cipherInput = context->EvalBootstrap(cipherInput);

    plainReLU(plainInput);


    //  Executing third linear layer
    std::cout << "Executing third linear layer..." << std::endl;
    cipherInput = matrix_multiplication(lin2_weights, cipherInput, context);
    pl = context->MakeCKKSPackedPlaintext(lin2_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
    std::cout << "Done!" << std::endl;

    plainInput = plain_matrix_multiplication(lin2_weights, plainInput);
    plainInput = plain_addition(plainInput, lin2_bias);

    //  Min and Max values for ReLU after third linear layer
    std::cout << "Third Relu...";
    xMin = -0.06039716675877571, xMax = 0.06716714799404144;
    cipherInput = evalRelu(cipherInput, xMin, xMax);
    std::cout << "Done!" << std::endl;

    // cipherInput = context->EvalBootstrap(cipherInput);

    plainReLU(plainInput);


    //  Executing fourth linear layer
    std::cout << "Executing fourth linear layer...";
    cipherInput = matrix_multiplication(lin3_weights, cipherInput, context);
    pl = context->MakeCKKSPackedPlaintext(lin3_bias);
    cipherInput = context->EvalAdd(pl, cipherInput);
    std::cout << "Done!" << std::endl;

    plainInput = plain_matrix_multiplication(lin3_weights, plainInput);
    plainInput = plain_addition(plainInput, lin3_bias);

    std::cout << "Level after all operations is: " << cipherInput->GetLevel() << std::endl << std::endl;

    Plaintext result;
    context->Decrypt(cipherInput, keys.secretKey, &result);
    result->SetLength(10);
    std::cout << "Output of the neurons was: " << result;

    auto resultVec = result->GetRealPackedValue();
    auto it = std::minmax_element(resultVec.begin(), resultVec.end());
    int res = std::distance(resultVec.begin(), it.second);
    std::cout << "The model predicted a " << res << ", the input file was called " << filename << std::endl;

    std::cout << "Plain output of the operations is: [\t";

    for (auto elem : plainInput)
        std::cout << elem << "\t";

    std::cout << "]" << std::endl;
    return 0;
}



Ciphertext<DCRTPoly> evalRelu (Ciphertext<DCRTPoly> cipher, double xMin, double xMax) {
    auto context = cipher->GetCryptoContext();

    return context->EvalChebyshevFunction([](double x) -> double {return x < .0 ? 0 : x;}, cipher, xMin, xMax, 3);
}

void print_shape(std::vector<std::vector<double>> matrix) {
	unsigned int shape0 = matrix[0].size();
	unsigned int shape1 = matrix.size();

	std::cout << "Shape is (" << shape1 << ", " << shape0 << ")" << std::endl;
}


void plainReLU (std::vector<double>& input) {
    for (auto& elem : input) {
        elem = elem < .0 ? .0 : elem;
    }
}


// double time_in_seconds (std::chrono::time_point<std::chrono::system_clock> start,
//                         std::chrono::time_point<std::chrono::system_clock> end) {
//     return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1E-9;
// }

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

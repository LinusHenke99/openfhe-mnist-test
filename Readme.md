# [OpenFHE](https://github.com/openfheorg/openfhe-development) mnist
This repository is dedicated to recreating the cryptonet neural network in OpenFHE in order to introduce fully homorphic
encryption. The project requires the following packages

## Installation
This is a cmake project, which is thus installed with
```
mkdir build && cd build
cmake ..
make 
make install
```
The binaries can then be executed from the `bin` folder in the source directory.

## Programs

### Keygen
Keygen can be used to generate the keys required for encryption/decryption and the operations required for carrying out
the neural network. The program's arguments are
```
<multiplication depth (should be set to 9 for the neural network to work)> <mod size> <first mod size> <security level>
```

### Encrypt
Encrypt can be used after the keygen program, in order to encrypt a $32\times 32$ mnist image stored in a .csv file into
an OpenFHE ciphertext. Example images can be found in the `source/img` folder. The path to the image file should be 
given as an argument to the program.

### Test Mnist
Do a test run for the neural network. The file containing the ciphertext on which inference should be made should be 
given as an argument.

### Eval Mnist

### Test Matmul

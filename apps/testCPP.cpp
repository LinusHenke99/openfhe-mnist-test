#include <iostream>
#include <bitset>


int main() {
    int i = 4;

    typedef std::bitset<sizeof(int)> bitChecker;

    std::cout << "Bit set: " << bitChecker(i).test(2) << std::endl;

    return 0;
}
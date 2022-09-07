#include "SimpleNeuralNetwork.h"

#include <vector>
#include <iostream>

int main() {
    SimpleNeuralNetwork net2({2,4,4,1});
    std::vector<float> vGenom = net2.getGenom();
    if (vGenom.size() != 30) {
        std::cout << "Expected 30, but got " << vGenom.size() << std::endl;
        return 1;
    }
    return 0;
}
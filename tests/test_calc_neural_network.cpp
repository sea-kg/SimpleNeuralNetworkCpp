#include "SimpleNeuralNetwork.h"

#include <vector>
#include <iostream>

int main() {
    SimpleNeuralNetwork net2({2,4,4,1});
    std::vector<float> vGenom = net2.getGenom();
    for (int i = 0; i < vGenom.size(); ++i) {
        vGenom[i] = 1.0f;
    }
    net2.setGenom(vGenom);
    float res = net2.calc({10,20})[0];
    if (res != 480) {
        std::cout << "Expected 480, but got " << res << std::endl;
        return 1;
    }
    return 0;
}
# SimpleNeuralNetworkCpp

Simple Neural Network C++

Features:

* You can use build-in genetic algorithm for learning neural network
* You can export to c++ function teached neural network


Sample (teach neural network for sum):

```cpp
#include <stdint.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>

#include "SimpleNeuralNetwork.h"
#include "test_sum_numbers_func.h"

void initTrainingData(SimpleNeuralTrainingItemList &trainingData, int nTrainingData) {
    // init training data
    for (int i = 0; i < nTrainingData; ++i) {
        float in0 = static_cast<float>(std::rand() % 100);
        float in1 = static_cast<float>(std::rand() % 100);
        trainingData.addItem({in0, in1}, {in0 + in1});
    }
}

int main(int argc, char *argv[]) {
	std::srand(std::time(nullptr));

    constexpr int nNumberOfIn = 2;
    constexpr int nNumberOfOut = 1;

    SimpleNeuralTrainingItemList trainingData(nNumberOfIn,nNumberOfOut);

    constexpr size_t nTrainingDataSize = 1000;
    initTrainingData(trainingData, nTrainingDataSize);

	SimpleNeuralNetwork *pNet = new SimpleNeuralNetwork({
        trainingData.getNumberOfIn(),
        64,64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    constexpr int nBetterSpecimens = 30;
    constexpr int nMutateSpecimens = 40;
    constexpr int nMixSpecimens = 40;
    SimpleNeuralGenomList genoms(nBetterSpecimens, nMutateSpecimens, nMixSpecimens);
    genoms.fillRandom(pNet);
    genoms.calculateRatingForAll(pNet, &trainingData);

    constexpr int nMaxGenerations = 100;
    constexpr float nConditionRatingStop = 0.1f;
    int n = 0;
    while (genoms.getBetterRating() > nConditionRatingStop && n < nMaxGenerations) {
        ++n;

        auto start = std::chrono::steady_clock::now();
        genoms.sort(); // better generations will be on the top

        std::cout << " ------- Generation " << n << " ------- " << std::endl;
        genoms.printFirstRatings(3);
        genoms.mutateAndMix(pNet);

        // calc rating
        genoms.calculateRatingForMutatedAndMixed(pNet, &trainingData);

        auto end = std::chrono::steady_clock::now();
        std::cout
            << "Elapsed time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms" << std::endl
        ;
        std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
    }

    pNet->setGenom(genoms.getBetterGenom().getGenom());

    // export to c++ code
    pNet->exportToCppFunction("SumNumbers.cpp", "sum_numbers");

    // test
    for (int i = 0; i < 10; i++) {
        float x = (std::rand() % 200) - 100;
        float y = (std::rand() % 200) - 100;
        std::cout << x << " + " << y << " = " << pNet->calc({x,y})[0] << ", expected (" << int(x+y) << ") " << std::endl;
    }

    // write genom to file "best_genom.txt"
    const std::vector<float> &vBetterGenom = genoms.list()[0].getGenom();
    std::ofstream file;
    file.open("best_genom.txt", std::ofstream::out);
    for (int i=0; i < vBetterGenom.size(); ++i) {
        file << vBetterGenom[i] << " ";
    }
    file << std::endl;
    file.close();

	return 0;
}
```

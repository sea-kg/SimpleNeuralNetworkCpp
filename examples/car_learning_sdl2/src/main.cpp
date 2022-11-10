/*
MIT License

Copyright (c) 2022 Evgenii Sopov (mrseakg@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdint.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <fstream>
#include <string>

#include "car_simulator.h"
#include "SimpleNeuralNetwork.h"

void initTrainingData(SimpleNeuralTrainingItemList &trainingData) {
    std::string sFilename = "examples/car_learning/data.txt";
    std::cout << "Read training data from " << sFilename << std::endl;
    std::ifstream data;
    data.open(sFilename.c_str(), std::ios_base::in);  // open data
    for (std::string sLine; std::getline(data, sLine); ) {
        std::istringstream iLine(sLine);      //make a stream for the line itself
        std::vector<float> vIn;
        for (int i = 0; i < 25; i++) {
            float x;
            iLine >> x;
            vIn.push_back(x);
        }
        float out1;
        iLine >> out1;
        float out2;
        iLine >> out2;
        trainingData.addItem(vIn, {out1, out2});
    }
}

int main(int argc, char *argv[]) {
	std::srand(std::time(nullptr));
    
    auto *pCarSimulator = new CarSimulator();
    if (!pCarSimulator->initSDL2()) {
        return -1;
    }

    return pCarSimulator->start();


    constexpr int nNumberOfIn = 25;
    constexpr int nNumberOfOut = 2;

    SimpleNeuralTrainingItemList trainingData(nNumberOfIn,nNumberOfOut);
    initTrainingData(trainingData);

	SimpleNeuralNetwork *pNet = new SimpleNeuralNetwork({
        trainingData.getNumberOfIn(),
        64, 128, 64, // middle layers
        // 64, 10, 64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    constexpr int nBetterSpecimens = 30;
    constexpr int nMutateSpecimens = 40;
    constexpr int nMixSpecimens = 40;
    SimpleNeuralGenomList genoms(nBetterSpecimens, nMutateSpecimens, nMixSpecimens);
    genoms.fillRandom(pNet); // TODO fill can be randomly, no need net
    genoms.calculateRatingForAll(pNet, &trainingData);

    constexpr int nMaxGenerations = 5000;
    constexpr float nConditionRatingStop = 2.0f;
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

    const std::vector<float> &vBetterGenom = genoms.list()[0].getGenom();
    
    std::ofstream file;
    file.open("best_genom.txt", std::ofstream::out | std::ofstream::app);
    for (int i=0; i < vBetterGenom.size(); ++i) {
        file << vBetterGenom[i] << " ";
    }
    file << std::endl;
    file.close();

    std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
	return 0;
}
